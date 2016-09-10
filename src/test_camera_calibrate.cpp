/*
 * test_camera_calibrate.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: lzp
 */


#include "camera_calibrate.h"


static void help() {
	cout<<"/**************************************/"<<endl;
	cout<<"Arguments: "<<endl
			<<"chessBoard_width chessBoard_height "
			<<"image_list_xml camera_param_yml "
			<<endl;
	cout<<"/**************************************/"<<endl;
}

int main(int argc, char* argv[]){

	if(argc!=5){
		help();
		return 0;
	}

	int width=atoi(argv[1]);
	int height=atoi(argv[2]);

	bool ok=camera_calibrate(width, height, argv[3], argv[4]);

	if(ok){
		cout<<"Calibrate successfully!"<<endl;
		cout<<"The parameter file is written in "<<argv[4]<<endl;
	}else{
		cout<<"Calibrate failed!"<<endl;
	}
}
