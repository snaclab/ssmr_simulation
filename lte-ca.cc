/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jaume Nin <jnin@cttc.es>
 */

#include <iostream>
#include <sstream>
#include <string>

#include <ns3/object.h>
#include <ns3/spectrum-interference.h>
#include <ns3/spectrum-error-model.h>
#include <ns3/log.h>
#include <ns3/test.h>
#include <ns3/simulator.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include "ns3/radio-bearer-stats-calculator.h"
#include <ns3/constant-position-mobility-model.h>
#include <ns3/eps-bearer.h>
#include <ns3/node-container.h>
#include <ns3/mobility-helper.h>
#include <ns3/net-device-container.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-helper.h>
#include "ns3/string.h"
#include "ns3/double.h"
#include <ns3/lte-enb-phy.h>
#include <ns3/lte-ue-phy.h>
#include <ns3/boolean.h>
#include <ns3/enum.h>
#include <ns3/my-global.h>
#include <math.h>
#include <fstream>
#include <ns3/rectangle.h>
#include <ns3/random-direction-2d-mobility-model.h>
#include <time.h>
#include <vector>
#include <ns3/ue-position-allocator.h>
#include <stdlib.h>
#include <fstream>
//NS_LOG_COMPONENT_DEFINE ("LenaTestPfFfMacCheduler");/*????*/

using namespace ns3;
using namespace std;
using std::vector;

void delay(int sec);
void Simulation_toBS(int seed, int numberofuser);
void Simulation_ToUE(int seed,int x[],int y[], int numberofuser);
void RandInit(int seed);
void Pause();
void getpositionfromfile();

const int UEtoRelay = 1;
const int RelaytoBs = 0;

const int pos_BSx = 250 , pos_BSy = 250;
const int pos_USERx = 400, pos_USERy = 250;
const int looptimes = 1;
const int relay_amongX= 120, relay_amongY=120;
const int minX = 190 , minY = 190;
const double simulation_time =1.1;

 int density_user =2; //include source node

void read_position(int x[] ,int y[], int loop){
				FILE *myFile;
			myFile = fopen("pos_e.txt","r");

			if(myFile){
				for (int j = 0;j<loop+1;j++){
					fscanf(myFile, "%d", &x[j]);
					fscanf(myFile, "%d", &y[j]);
				}	
				for (int j = 0;j<loop+1;j++){
					printf("%d,", x[j]);
					printf("%d\n", y[j]);
				}	
			}else{
				cout <<"!!!!!Can not open the file!!!!" << endl;
			}
			fclose(myFile);
}

void Read_ulRlcStats(double Tx[],double Rx[],int Ux,int mode){
	FILE *myFile;
	myFile = fopen("UlRlcStats.txt", "r");
//myFile = fopen("test.txt", "r");

//read file into array
	int User = Ux;
	int i,j;
	char c[10000];
	double pass[1];
	double temp_Tx[1] = {};
	double temp_Rx[1] = {};
	fscanf(myFile,"%[^\n]",c);
	//printf("\n---------------\n");

	for (i = 0; i < User; i++)
	{
		for (j=0;j<7;j++){
			fscanf(myFile, "%lf", &pass[0]);
		}	
		if(mode==0){
			fscanf(myFile, "%lf", &Tx[i]);
			//Tx[i] += temp_Tx[0]/1000;
		 }else{
		 	fscanf(myFile, "%lf", &pass[0]);
		 }

			fscanf(myFile, "%lf", &pass[0]);

		if(mode==1){
			fscanf(myFile, "%lf", &Rx[i]);
			//Rx[i] += temp_Rx[0]/1000;
		 }
		 else{
		 	fscanf(myFile, "%lf", &pass[0]);
		 }

		for (j=0;j<8;j++){
			fscanf(myFile, "%lf", &pass[0]);
		}
	}

	for(int i =0 ; i<User;i++){

		//printf("%lf\t%lf\n",Tx[i],Rx[i]);
	}

	fclose(myFile);

}


int main(int argc, char *argv[])
{	int numberofuser = g_nUsers; 
	numberofuser = 2;
	double RelaytoBs_Bytes[100] ={};
	double UEtoRelay_Bytes[100] ={};

	// set root directory
	sprintf(g_sRootDir, "data_b%d_u%d", g_band, g_nUsers);/*band??, # of users*/
	char cmd_mkdir[64];
	sprintf(cmd_mkdir, "mkdir -m 777 %s", g_sRootDir);/*create a dir named g_sRootDir(data_bxx_uxx)*/
	system(cmd_mkdir);	

	// get position from pos_e.txt
	//----------------------------------------------------------	
	// output simulation information
	cout << "g_band = " << g_band << endl;
	cout << "g_nUsers = " << g_nUsers << endl;
	switch(g_nScene)
	{
		case 0: cout << "g_nScene = Uniform" << endl; break;
		case 1: cout << "g_nScene = Centralize" << endl; break;
		case 2: cout << "g_nScene = Distributed" << endl; break;
		case 3: cout << "g_nScene = HotZone" << endl;
	}
	cout << "seed = " << g_nSeedS << " ~ " << g_nSeedE << endl; 
	
	// set data directory	
	for (int i = g_nSeedS; i <= g_nSeedE; i++) /*define g_nSeedS 1, seed start number, g_nSeedE 50*/
	{
		char dir[64];                
		sprintf(dir, "%s/seed_%d", g_sRootDir, i);/*create a dir in g_sRootDir/ */
		char cmd_mkdir[64];
		sprintf(cmd_mkdir, "mkdir -m 777 %s", dir);
		system(cmd_mkdir);
	}	

    Pause();
	
	time_t start, end;
	time(&start);
	
	// run simulation
	int count = 0;
	for(int i=1;i<3;i++){
		int x[numberofuser],y[numberofuser];
		for(int i=0;i<looptimes;i++){
			for (int i = g_nSeedS; i <= g_nSeedE; i++)
			{	
				Simulation_toBS(i,numberofuser); // round 2 error!!
				Read_ulRlcStats(RelaytoBs_Bytes,UEtoRelay_Bytes,numberofuser,RelaytoBs);



				read_position(x,y,numberofuser);


				Simulation_ToUE(i,x,y,numberofuser);
				Read_ulRlcStats(RelaytoBs_Bytes,UEtoRelay_Bytes,numberofuser,UEtoRelay);

			}	
			//std::cout << (double)((i+1)*100/looptimes) << "%..." << std::endl;		
		
	
		time(&end);
		sprintf(g_sDir, "%s/seed_%d", g_sRootDir, g_nSeed);/*g_nSeed = i*/	
		char posFile1[64];
		sprintf(posFile1, "%s/Result%d_%d.txt", g_sDir, numberofuser, count );

		ofstream fout_result(posFile1);
		for(int i=0;i<numberofuser;i++){
	 		printf("%lf\t%lf\n", UEtoRelay_Bytes[i],RelaytoBs_Bytes[i]);
	 		fout_result << UEtoRelay_Bytes[i] << "\t" << RelaytoBs_Bytes[i] << std::endl;
			}
		count++;
		memset(UEtoRelay_Bytes,0,100);
		memset(RelaytoBs_Bytes,0,100);
		}
		if(i==0){
			numberofuser = 1;
			density_user = 1;
		}
		count=0;		
		numberofuser += 5;
		density_user +=5;
	
	}
	double dif = difftime(end, start);
	std::cout << "Simulation finished! (" << dif << "s)" << std::endl;
	return 0;
}

void Simulation_toBS(int seed, int numberofuser)
{
	g_count = 0;
	
	// initialize random procedure
	RandInit(seed);/*input seed from g_nSeedS to g_nSeedE*/
	
	// set data directory
	sprintf(g_sDir, "%s/seed_%d", g_sRootDir, g_nSeed);/*g_nSeed = i*/	
	

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	lteHelper->SetAttribute("PathlossModel", StringValue("ns3::LTESpectrumPropagationLossModel"));

	// Create Nodes: eNodeB and UE
	NodeContainer enbNodes;
	NodeContainer ueNodes;
	enbNodes.Create (1);
	ueNodes.Create (numberofuser); // # of users

	/***** Install Mobility Model *****/
	MobilityHelper mobility;
	// eNB Mobility Model
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install (enbNodes);

	// UE Mobility Model
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(ueNodes);
	// Create Devices and install them in the Nodes (eNB and UE)
	NetDeviceContainer enbDevs;
	NetDeviceContainer ueDevs;

	lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
	
	enbDevs = lteHelper->InstallEnbDevice(enbNodes);/*install Nodes to NetDevice*/
	ueDevs = lteHelper->InstallUeDevice(ueNodes);

	// Attach a UE to a eNB
	lteHelper->Attach(ueDevs, enbDevs.Get(0));

	// Activate an EPS bearer
	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
	EpsBearer bearer(q);
	lteHelper->ActivateEpsBearer(ueDevs, bearer, EpcTft::Default ());

	Ptr<LteEnbNetDevice> lteEnbDev = enbDevs.Get(0)->GetObject<LteEnbNetDevice> ();
	Ptr<LteEnbPhy> enbPhy = lteEnbDev->GetPhy();
	enbPhy->SetAttribute("TxPower", DoubleValue(46.0));
	enbPhy->SetAttribute("NoiseFigure", DoubleValue(5.0));



	 Ptr<ConstantPositionMobilityModel> mm = enbNodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
	 mm->SetPosition(Vector(pos_BSx, pos_BSy, 0.0));
	 Vector pos = mm->GetPosition();
	
	char posFile1[64], posFile2[64];
	sprintf(posFile1, "%s/pos_s%d.txt", g_sDir, numberofuser );
	sprintf(posFile2, "%s/pos_e%d.txt", g_sDir, numberofuser );
	
	UEPositionAllocator upa(g_nScene); // 0 = Uniform, 1 = Centralize, 2 = Distributed, 3 = HotZone
	
	ofstream fout(posFile1);
	fout << pos_BSx << "\t" << pos_BSy << std::endl; // Base station
	// Set UEs' position and power

	for (int i = 0; i < numberofuser; i++)
	{  if(i<density_user){
		srand(time(NULL));
		int x = rand()%relay_amongX+minX;
		int y = rand()%relay_amongY+minY;
		Ptr<LteUeNetDevice> lteUeDev = ueDevs.Get (i)->GetObject<LteUeNetDevice> ();
		Ptr<LteUePhy> uePhy = lteUeDev->GetPhy ();/*return a pointer to the physical layer*/
		uePhy->SetAttribute("TxPower", DoubleValue(23.0));
		uePhy->SetAttribute("NoiseFigure", DoubleValue(9.0));
		
		Ptr<ConstantPositionMobilityModel> mm = ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
		//Ptr<RandomDirection2dMobilityModel> mm = ueNodes.Get(i)->GetObject<RandomDirection2dMobilityModel> ();
		pair<int,int> UE_pos = upa.GetUEPos(); // get random position
		if(i==0)
		mm->SetPosition(Vector(pos_USERx, pos_USERy, 0.0));
		else
		mm->SetPosition(Vector(x, y, 0.0));
		
		//mm->SetPosition(Vector(UE_pos.first, UE_pos.second, 0.0));
		Vector pos = mm->GetPosition();
		fout << pos.x << "\t" << pos.y << std::endl;
		delay(10);
	}else{
		srand(time(NULL));
		int x = rand()%335+100;
		int y = rand()%250+200;
		Ptr<LteUeNetDevice> lteUeDev = ueDevs.Get (i)->GetObject<LteUeNetDevice> ();
		Ptr<LteUePhy> uePhy = lteUeDev->GetPhy ();/*return a pointer to the physical layer*/
		uePhy->SetAttribute("TxPower", DoubleValue(23.0));
		uePhy->SetAttribute("NoiseFigure", DoubleValue(9.0));
		
		Ptr<ConstantPositionMobilityModel> mm = ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
		//Ptr<RandomDirection2dMobilityModel> mm = ueNodes.Get(i)->GetObject<RandomDirection2dMobilityModel> ();
		pair<int,int> UE_pos = upa.GetUEPos(); // get random position
		if(i==0)
		mm->SetPosition(Vector(pos_USERx, pos_USERy, 0.0));
		else
		mm->SetPosition(Vector(x, y, 0.0));
		
		//mm->SetPosition(Vector(UE_pos.first, UE_pos.second, 0.0));
		Vector pos = mm->GetPosition();
		fout << pos.x << "\t" << pos.y << std::endl;
		delay(10);
		}
		
	}

		
	
	//std::cout << std::endl;
	fout.close();
	double simulationTime = simulation_time; // 0.4 10
	Simulator::Stop(Seconds(simulationTime));

	lteHelper->EnableMacTraces();
	lteHelper->EnableRlcTraces();
	lteHelper->EnablePdcpTraces();

	time_t start, end;
	time(&start);
	Simulator::Run();
	time(&end);
	double dif = difftime(end, start);
	//cout << "Time = " << dif << endl;/*Simulation spend time*/
	ofstream fout2("pos_e.txt");
	fout2 << pos_BSx << "\t" << pos_BSy << std::endl; // Base station
	for (int i = 0; i < numberofuser; i++)
	{	
		Ptr<ConstantPositionMobilityModel> mm = ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
		//Ptr<RandomDirection2dMobilityModel> mm = ueNodes.Get(i)->GetObject<RandomDirection2dMobilityModel> ();
		Vector pos = mm->GetPosition();
		fout2 << pos.x << "\t" << pos.y << std::endl;
	}
	fout2.close();
	Simulator::Destroy();
	
	std::cout << "Simulation_toBS finished! (" << dif << "s)" << std::endl;
}



void Simulation_ToUE(int seed,int x[],int y[], int numberofuser)
{
	g_count = 0;
	
	// initialize random procedure
	RandInit(seed);/*input seed from g_nSeedS to g_nSeedE*/
	
	// set data directory
	sprintf(g_sDir, "%s/seed_%d", g_sRootDir, g_nSeed);/*g_nSeed = i*/	
	
	/**
	* Initialize Simulation Scenario: 1 eNB and g_nUsers UEs
	*/


	// set using bands of CCs
	if (g_band == 0)
	{
		band[0] = 0;
		band[1] = 0;
		band[2] = 0;
		band[3] = 0;
	}
	else if (g_band == 1)
	{
		band[0] = 0;
		band[1] = 0;
		band[2] = 1;
		band[3] = 1;
	}
	
	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	lteHelper->SetAttribute("PathlossModel", StringValue("ns3::LTESpectrumPropagationLossModel"));

	// Create Nodes: eNodeB and UE
	NodeContainer enbNodes;
	NodeContainer ueNodes;
	enbNodes.Create (1);
	ueNodes.Create (numberofuser); // # of users

	/***** Install Mobility Model *****/
	MobilityHelper mobility;
	// eNB Mobility Model
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install (enbNodes);

	// UE Mobility Model
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	// mobility.SetMobilityModel("ns3::RandomDirection2dMobilityModel",
 //                             "Bounds", RectangleValue(Rectangle(-500, 500, -500, 500)),
 //                             "Pause", RandomVariableValue(ConstantVariable(0.000001)));
	
	//Ptr<PositionAllocator> positionAlloc = CreateObject<RandomDiscPositionAllocator> ();
	//positionAlloc->SetAttribute("Rho", RandomVariableValue(UniformVariable(250, 500))); // radius
	//mobility.SetPositionAllocator(positionAlloc);
	mobility.Install(ueNodes);

	// Create Devices and install them in the Nodes (eNB and UE)
	NetDeviceContainer enbDevs;
	NetDeviceContainer ueDevs;

	lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
	//lteHelper->SetSchedulerType("ns3::TtaFfMacScheduler");
	
	//lteHelper->SetSchedulerType("ns3::MyFfMacScheduler");
		
	enbDevs = lteHelper->InstallEnbDevice(enbNodes);/*install Nodes to NetDevice*/
	ueDevs = lteHelper->InstallUeDevice(ueNodes);

	// Attach a UE to a eNB
	lteHelper->Attach(ueDevs, enbDevs.Get(0));

	// Activate an EPS bearer
	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
	EpsBearer bearer(q);
	lteHelper->ActivateEpsBearer(ueDevs, bearer, EpcTft::Default ());

	Ptr<LteEnbNetDevice> lteEnbDev = enbDevs.Get(0)->GetObject<LteEnbNetDevice> ();
	Ptr<LteEnbPhy> enbPhy = lteEnbDev->GetPhy();
	enbPhy->SetAttribute("TxPower", DoubleValue(23.0));
	enbPhy->SetAttribute("NoiseFigure", DoubleValue(9.0));



	 Ptr<ConstantPositionMobilityModel> mm = enbNodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
	 mm->SetPosition(Vector(x[1], y[1], 0.0));
	 Vector pos = mm->GetPosition();
	//std::cout << x[1] << " " << y[1] << std::endl;
	
	char posFile1[64], posFile2[64];
	//sprintf(posFile1, "%s/pos_s.txt", g_sDir);
	sprintf(posFile2, "%s/pos_e%d.txt", g_sDir,numberofuser);
	
	UEPositionAllocator upa(g_nScene); // 0 = Uniform, 1 = Centralize, 2 = Distributed, 3 = HotZone
	
	for (int i = 0; i < numberofuser; i++)
	{ 	
		Ptr<LteUeNetDevice> lteUeDev = ueDevs.Get (i)->GetObject<LteUeNetDevice> ();
		Ptr<LteUePhy> uePhy = lteUeDev->GetPhy ();/*return a pointer to the physical layer*/
		if(i==0){
		uePhy->SetAttribute("TxPower", DoubleValue(46.0));
		uePhy->SetAttribute("NoiseFigure", DoubleValue(5.0));
		}else{
		uePhy->SetAttribute("TxPower", DoubleValue(23.0));
		uePhy->SetAttribute("NoiseFigure", DoubleValue(9.0));
		}
		Ptr<ConstantPositionMobilityModel> mm = ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
		//Ptr<RandomDirection2dMobilityModel> mm = ueNodes.Get(i)->GetObject<RandomDirection2dMobilityModel> ();
		pair<int,int> UE_pos = upa.GetUEPos(); // get random position
		if(i==0){
		mm->SetPosition(Vector(x[0], y[0], 0.0));
		//cout <<x[0]<<" "<<y[0]<<"\n";
		}
		else{
		mm->SetPosition(Vector(x[i+1], y[i+1], 0.0));
		//cout <<x[i+1]<<" "<<y[i+1]<<"\n";
		}
		//mm->SetPosition(Vector(UE_pos.first, UE_pos.second, 0.0));
		Vector pos = mm->GetPosition();
	//	fout << pos.x << "\t" << pos.y << std::endl;
		//delay(10);
	}
	//std::cout << std::endl;
	//fout.close();
	
	double simulationTime = simulation_time; // 0.4 10
	Simulator::Stop(Seconds(simulationTime));

	lteHelper->EnableMacTraces();
	lteHelper->EnableRlcTraces();
	lteHelper->EnablePdcpTraces();

	time_t start, end;
	time(&start);
	Simulator::Run();
	time(&end);
	double dif = difftime(end, start);
	//cout << "Time = " << dif << endl;/*Simulation spend time*/
	
	ofstream fout2(posFile2);
	fout2 << x[1] << "\t" << y[1] << std::endl; // Base station
	for (int i = 0; i < numberofuser; i++)
	{
		Ptr<ConstantPositionMobilityModel> mm = ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
				
		//printf("%d",xX);
		Vector pos = mm->GetPosition();

		fout2 << pos.x << "\t" << pos.y << std::endl;
	}
	fout2.close();

	Simulator::Destroy();
	
	std::cout << "Simulation_ToUE finished! (" << dif << "s)" << std::endl;
}

void RandInit(int seed)
{
	g_nSeed = seed;
	srand(g_nSeed);/*g_nSeed is the current Seed*/
	SeedManager::SetSeed(g_nSeed);/*define in src/core/model/random-variable.h*/
}

void Pause()
{
	int temp;
//	cin >> temp;
}

void delay(int sec){
	for(int i = 0;i<100000;i++)
		for(int j = 0; j<10000;j++)
		{}

}


