#!/bin/bash

DIR_BENCHMARK=`dirname $0`

info_view()
{
    echo "*****************************************************"
    echo "***                                               ***"
    echo "***                BENCHMARK TEST                 ***"
    echo "***                                               ***"
    echo "*****************************************************"
}

info_view
echo "*****************************************************"
echo "unixbench test:                                  1"
echo "glmark2 test:                          2"
echo "*****************************************************"

read -t 30 BENCHMARK_CHOICE

unixbench_test()
{
	bash ${DIR_BENCHMARK}/unixbench_test.sh
}

glmark2_test()
{
	bash ${DIR_BENCHMARK}/glmark2_test.sh
}


case ${BENCHMARK_CHOICE} in
	1)
		unixbench_test
		;;
	2)
		glmark2_test
		;;
	*)
		echo "not found your input."
		;;
esac
