/******************************************************************************
 *
 *  Copyright (C) 2020-2021 Rockchip Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <utils/Log.h>
#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "zdm"
#endif

#define HCI_GRP_LINK_CONTROL_CMDS       (0x01 << 10)            /* 0x0400 */
#define HCI_GRP_HOST_CONT_BASEBAND_CMDS (0x03 << 10)            /* 0x0C00 */

/* Commands of HCI_GRP_HOST_CONT_BASEBAND_CMDS */
#define HCI_RESET                       (0x0003 | HCI_GRP_HOST_CONT_BASEBAND_CMDS)
/* Commands of HCI_GRP_LINK_CONTROL_CMDS group */
#define HCI_Change_Connection_Packet_Type	(0x000F | HCI_GRP_LINK_CONTROL_CMDS)

#define HCIC_PARAM_SIZE_CHANGE_CONN_TYPE     4
#define HCIC_HEADER_SIZE	3//opcode(2)+paralen(1)

/* Command opcode pack/unpack */
#define cmd_opcode_pack(ogf, ocf)	((uint16_t)((ocf & 0x03ff)|(ogf << 10)))		

#define MAXSLEEP 4
static const int SERV_ADDR = 0x7f000001;
static const uint16_t SERV_PORT = 8873;

#define BIT(x) (1<<x)

#define NO_2_DH1         BIT(1)
#define NO_3_DH1         BIT(2)
#define DM1              BIT(3) //always enable
#define DH1              BIT(4)
//#define HV1              BIT(5)
//#define HV2              BIT(6)
//#define HV3              BIT(7)
#define NO_2_DH3         BIT(8)
#define NO_3_DH3         BIT(9)
#define DM3              BIT(10)
#define DH3              BIT(11)
#define NO_2_DH5         BIT(12)
#define NO_3_DH5         BIT(13)
#define DM5              BIT(14)
#define DH5              BIT(15)


#define b_NOALL (DM1|NO_2_DH1|NO_3_DH1|NO_2_DH3|NO_3_DH3|NO_2_DH5|NO_3_DH5)//keep DM1 available
/*#define b_DM1 	(b_NOALL|DM1)//always enabled
#define b_DH1   (b_NOALL|DH1)
#define b_2DH1  (b_NOALL&~NO_2_DH1)
#define b_3DH1  (b_NOALL&~NO_3_DH1)
#define b_DM3   (b_NOALL|DM3)
#define b_DH3   (b_NOALL|DH3)
#define b_2DH3  (b_NOALL&~NO_2_DH3)
#define b_3DH3  (b_NOALL&~NO_3_DH5)
#define b_DM5   (b_NOALL|DM5)
#define b_DH5   (b_NOALL|DH5)
#define b_2DH5  (b_NOALL&~NO_2_DH5)
#define b_3DH5  (b_NOALL&~NO_3_DH5)*/

#define LOGI(param, ...) do { ALOGI(param, ##__VA_ARGS__); printf(param, ##__VA_ARGS__);}while(0)
#define LOGD(param, ...) do { ALOGD(param, ##__VA_ARGS__); printf(param, ##__VA_ARGS__);}while(0)
#define LOGW(param, ...) do { ALOGW(param, ##__VA_ARGS__); printf(param, ##__VA_ARGS__);}while(0)
#define LOGE(param, ...) do { ALOGE(param, ##__VA_ARGS__); printf(param, ##__VA_ARGS__); }while(0)

struct bredr_t {
	char *type;
	uint16_t  mask;
	bool inverse;
};

struct bredr_t bredr[]={
    {"DM1"   ,DM1	 	, false	},
    {"DH1"   ,DH1  		, false	},
    {"2-DH1" ,NO_2_DH1 	, true	},
    {"3-DH1" ,NO_3_DH1 	, true	},
    {"DM3"   ,DM3  		, false	},
    {"DH3"   ,DH3  		, false	},
    {"2-DH3" ,NO_2_DH3 	, true	},
    {"3-DH3" ,NO_3_DH3 	, true	},
    {"DM5"   ,DM5  		, false	},
    {"DH5"   ,DH5  		, false	},
    {"2-DH5" ,NO_2_DH5 	, true	},
    {"3-DH5" ,NO_3_DH5 	, true	},
    { NULL	 ,0 		, false }
};

static bool findBrEdrType(const char *string, bool* inverse, uint16_t* bitmask)
{
    if (!string)
        goto FAIL;

    for(int i=0; bredr[i].type; i++){
        if(!strcasecmp(bredr[i].type, string)){
			LOGD(" %s\n", bredr[i].type);
			*inverse = bredr[i].inverse;
			*bitmask = bredr[i].mask;
            return true;
        }
    }

FAIL:
    return false;
}

int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t alen)
{
	int numsec, fd;
	/*Try to connect with exponentail backoff*/

	for(numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
		if((fd = socket(domain, type, protocol)) < 0)
			return -1;
		}

		if(connect(fd, addr, alen) ==0) {
			/*connection accept*/
			return (fd);
		}
		close(fd);

	/* Delay before trying again*/
	if(numsec <= MAXSLEEP/2){
		sleep(numsec);
	}

	return -1;
}

static void usage(void)
{
	LOGD("bluedroidhci --- HCI tool to send hci command\n");
	LOGD("Usage:\n");
	LOGD("\tbluedroidhci [-h handle] [-i airtype(uint16)] [-t airtype1 airtype2 ...(string)] [--help]\n\tor\n");
	LOGD("\tbluedroidhci [-c|--cmd ogf ocf para....]\n");
	LOGD("\tavailable airtype:DM1 DH1 2-DH1 3-DH1 DM3 DH3 2-DH3 3-DH3 DM5 DH5 2-DH5 3-DH5\n");
	LOGD("\texample :bluedroidhci -h 0x80 -t DM1 DH1 2-DH3 3-DH3 2-DH5\n");
	LOGD("\texample :bluedroidhci -h 0x80 -i 0x380e\n");
	LOGD("\texample :to send hci raw command(hci reset, etc):\n");
	LOGD("\t\t bluedroidhci --cmd 0x03 0x0003\n");
}

static struct option long_options[] = {
	{"help",    0, 0,  0 },
	{"cmd",  required_argument, 0, 'c'},
	{0,         0,                 0,  0 }
	};

int main(int argc, char *argv[])
{
	struct sockaddr_in serverAdd;
	int fd;
	int total, opt, n;
	bool hciReset =0, hcicmd =0, inverse;
	uint16_t airType=b_NOALL, handle = 0x80, bitmask;
	int option_index = 0;
	uint16_t ocf, opcode;
	uint8_t ogf;

	unsigned char cmd[260] = {0}, *ptr = cmd;;//{ 0x01, 0x00, 0x03, 0x03, 0x0C, 0x00 };

	if(argc < 2){
		usage();
		//while(1){printf("while zdm, error = %d\n", errno);};
		exit(1);
	}

	memset(&serverAdd, 0, sizeof(struct sockaddr_in));
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = htonl(SERV_ADDR);
	serverAdd.sin_port = htons(SERV_PORT);

	fd = connect_retry(AF_INET, SOCK_STREAM, IPPROTO_TCP, (struct sockaddr *)&serverAdd, sizeof(serverAdd));

	if(fd < 0){
		printf("connection fail, error = %d\n", errno);
		goto MAIN_EXIT;
	}

	LOGD("connection successful!!!\n");
	
	while (1) {
		opt=getopt_long(argc, argv, "c:h:t:i:", long_options, &option_index);
		
		if (opt == -1)
		break;

		switch(opt) {
		case 0:
			LOGD("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			usage();
			goto MAIN_EXIT;
			break;
		case 'h':
			//printf("option %s\n", optarg);
			handle = (uint16_t)strtol(optarg, NULL, 16);
			break;

		case 't':
			LOGD("airType strings:");
		    LOGD("%s ", optarg);
			if(findBrEdrType(optarg, &inverse, &bitmask)){
				if(inverse)
					airType&=~bitmask;
				else
					airType|=bitmask;
			}
			break;

		case 'i':
			//printf("airType strings:%s \n", optarg);
			
			airType = (uint16_t)strtol(optarg, NULL, 16);
			printf("airType:0x%04x \n", airType);
			break;

		case 'c':
			hcicmd = 1;
			ogf = strtol(optarg, NULL, 16);
			break;

		default:
			usage();
			goto MAIN_EXIT;
		}
	}

	//n = argc - optind;

	/*if (n < 1) {
		usage();
		exit(1);
	}*/

	if(hcicmd){
		int i, len, paralen;
		ocf = strtol(argv[optind++], NULL, 16);
		opcode = cmd_opcode_pack(ogf, ocf);
		len = argc - optind;

		cmd[0] = 0x01;//cmd
		cmd[1] = (HCIC_HEADER_SIZE+ len)&0xff;//paralen
		cmd[2] = (HCIC_HEADER_SIZE+ len)>>8&0xff;//paralen
		cmd[3] = opcode&0xff;//opcode
		cmd[4] = opcode>>8&0xff;
		cmd[5] = len; //para len
		total = 3 + HCIC_HEADER_SIZE + cmd[5];
		
		ptr = &cmd[6];
		for(i= optind, paralen = 0; i < argc && paralen < (int)sizeof(cmd); i++, paralen++){
			*ptr++ = (uint8_t) strtol(argv[optind++], NULL, 16);
		}

		LOGD("paralen:%d , len =%d, %s\n", paralen, len, (paralen == len)?"ok":"some error!");
		LOGD("hci command:%02x (%02x %02x)", cmd[0], cmd[1], cmd[2]);
		for(int i = 0; i < len+3; i++){
			printf(" %02x", cmd[3+i]);
		}
		printf("\n");
	}
	else{
		//printf("optind:%d argc:%d", optind, argc);
		while (optind < argc) {
			const char *opt = argv[optind];
			printf("%s ", opt);
			if(findBrEdrType(opt, &inverse, &bitmask)){
				if(inverse)
					airType&=~bitmask;
				else
					airType|=bitmask;
			}
			optind++;
		}
	}
	LOGD("\n final airType :0x%04x, handle=0x%02x\n", airType, handle);

    if(hcicmd){
		#if 0
		cmd[0] = 0x01;//cmd
		cmd[1] = (HCIC_HEADER_SIZE+0)&0xff;//len
		cmd[2] = (HCIC_HEADER_SIZE+0)>>8&0xff;
		cmd[3] = HCI_RESET&0xff;//opcode
		cmd[4] = HCI_RESET>>8&0xff;
		cmd[5] = 0; //para
		total = 3 + HCIC_HEADER_SIZE;
		#endif
		
		if (write(fd, cmd, total) != total) {
			fprintf(stderr, "Failed to write reset command\n");
			goto MAIN_EXIT;
		}
	}
	else{
		cmd[0] = 0x01;//cmd
		cmd[1] = (HCIC_HEADER_SIZE+HCIC_PARAM_SIZE_CHANGE_CONN_TYPE)&0xff;//len
		cmd[2] = (HCIC_HEADER_SIZE+HCIC_PARAM_SIZE_CHANGE_CONN_TYPE)>>8&0xff;
		cmd[3] = HCI_Change_Connection_Packet_Type&0xff;//opcode
		cmd[4] = HCI_Change_Connection_Packet_Type>>8&0xff;
		cmd[5] = HCIC_PARAM_SIZE_CHANGE_CONN_TYPE;//opcode
		cmd[6] = handle&0xff;//opcode
		cmd[7] = handle>>8&0xff;
		cmd[8] = airType&0xff;//opcode
		cmd[9] = airType>>8&0xff;
		total = 3 + HCIC_HEADER_SIZE + HCIC_PARAM_SIZE_CHANGE_CONN_TYPE;
		
		if (write(fd, cmd, total) != total) {
			fprintf(stderr, "Failed to write reset command\n");
			goto MAIN_EXIT;
		}
	}

    //while(1);
    //usage();
	close(fd);
	LOGD("exit successful!!!\n");
	//usage();
	exit(0);//return 0;

MAIN_EXIT:
	close(fd);
	LOGD("exit unsuccessful!!!\n");
	//usage();
	exit(1);
}
