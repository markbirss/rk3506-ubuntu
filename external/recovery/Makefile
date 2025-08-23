PROJECT_DIR := $(shell pwd)
CC = gcc
PROM = recovery
UPDATE_ENGINE = updateEngine

all: $(PROM) $(UPDATE_ENGINE)
.PHONY : all

OBJ = recovery.o \
	default_recovery_ui.o \
	rktools.o \
	roots.o \
	bootloader.o \
	safe_iop.o \
	strlcpy.o \
	strlcat.o \
	rkupdate.o \
	sdboot.o \
	usbboot.o \
	mtdutils/mounts.o \
	mtdutils/mtdutils.o \
	mtdutils/rk29.o \
	minzip/DirUtil.o \
	update_engine/log.o

ifdef RecoveryNoUi
OBJ += noui.o
else
OBJ += ui.o\
	minzip/Hash.o \
	minzip/SysUtil.o \
	minzip/Zip.o \
	minui/events.o \
	minui/graphics.o \
	minui/resources.o \
	minui/graphics_drm.o
endif

CFLAGS += -I$(PROJECT_DIR) -I/usr/include -I/usr/include/libdrm/ -lc -DUSE_UPDATEENGINE=ON

ifdef RecoveryNoUi
CFLAGS += -lpthread -lbz2
else
CFLAGS += -lz -lpng -ldrm -lpthread -lcurl -lbz2
endif

UPDATE_ENGINE_OBJ = mtdutils/mounts.o \
	mtdutils/mtdutils.o \
	mtdutils/rk29.o \
	update_engine/rkbootloader.o \
	update_engine/download.o \
	update_engine/flash_image.o \
	update_engine/log.o \
	update_engine/main.o \
	update_engine/md5.o \
	update_engine/md5sum.o \
	update_engine/rkimage.o \
	update_engine/rktools.o \
	update_engine/rkboot.o \
	update_engine/crc.o \
	update_engine/update.o \
	update_engine/do_patch.o

recovery_version:
	@if ! command -v git > /dev/null; then \
		echo "Git is not installed. Setting default commit info."; \
		commit_info="-g<unknown>"; \
	else \
		COMMIT_HASH=$$(git rev-parse --verify --short HEAD 2> /dev/null); \
		if [ -z "$$COMMIT_HASH" ]; then \
			echo "Failed to get commit hash. Setting default commit info."; \
			commit_info="-g<unknown>"; \
		else \
			GIT_COMMIT_TIME=$$(git log -1 --format=%cd --date=format:%y%m%d 2> /dev/null); \
			if [ -z "$$GIT_COMMIT_TIME" ]; then \
				echo "Failed to get commit time. Setting default commit info."; \
				commit_info="-g$${COMMIT_HASH}-000000"; \
			else \
				commit_info="-g$${COMMIT_HASH}-$${GIT_COMMIT_TIME}"; \
			fi; \
		fi; \
	fi; \
	echo "#define GIT_COMMIT_INFO $${commit_info}" > recovery_autogenerate.h

$(PROM): $(OBJ)
	$(CC) -o $(PROM) $(OBJ) $(CFLAGS)

$(UPDATE_ENGINE): $(UPDATE_ENGINE_OBJ)
	$(CC) -o $(UPDATE_ENGINE) $(UPDATE_ENGINE_OBJ) $(CFLAGS)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.c recovery_version
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ) $(PROM) $(UPDATE_ENGINE_OBJ) $(UPDATE_ENGINE)

install:
	mkdir -p $(DESTDIR)/res/images $(DESTDIR)/usr/bin
	install -D -m 755 $(PROJECT_DIR)/recovery $(DESTDIR)/usr/bin/
	install -D -m 755 $(PROJECT_DIR)/updateEngine $(DESTDIR)/usr/bin/
	cp $(PROJECT_DIR)/res/images/* $(DESTDIR)/res/images/

