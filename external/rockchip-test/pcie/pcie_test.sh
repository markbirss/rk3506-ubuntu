#!/bin/bash

CURRENT_DIR=`dirname $0`

info_view()
{
	echo $CURRENT_DIR
	echo "*****************************************************"
	echo "***                                               ***"
	echo "***                 PCIE TEST                     ***"
	echo "***                                               ***"
	echo "*****************************************************"
}

info_view
echo "*****************************************************"
echo "pcie test:					  1"
echo "*****************************************************"

read -t 30 PCIE_CHOICE

pcie_test()
{
	bash ${CURRENT_DIR}/pcie_function_test.sh &
}

case ${PCIE_CHOICE} in
	1)
		pcie_test
		;;
	*)
		echo "not found your input."
		;;
esac
