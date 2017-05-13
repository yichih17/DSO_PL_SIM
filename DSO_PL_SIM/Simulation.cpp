#include"define.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<random>
#include<time.h>
#define UEnumber 50
#define outputPAT 1									//1: output PAT to txt file ; 0: store in program memory
#define outputUEinfo 0								//1: output UE information to txt file ; 0: store in program memory

using namespace std;

int LTECQIRange[] = { 1732, 1511, 1325, 1162, 1019, 894, 784, 688, 623, 565, 512, 449, 407, 357, 303 };
UE UEList[UEnumber];

template <class T>
void uniformdistribution(T* equip)
{
	std::random_device rd;							//integer random number generator that produces non-deterministic random numbers. 
	std::mt19937 gen(rd());							//Mersenne Twister 19937 generator, generate a random number seed
	std::uniform_real_distribution<> theta(0, 360);	//definition of a uniform distribution range, a random number between 0 and 360
	std::uniform_real_distribution<> k(0, 1);		//definition of a uniform distribution range, a random number between 0 and 1
	double r = radius_eNB * sqrt(k(gen));			//random a angle and random a radius, to gennerate a coordinate for UE
	double angel = (theta(gen));
	equip->coor_X = r * std::sin(angel);
	equip->coor_Y = r * std::cos(angel);
}

double exponentially_Distributed(double x)
{
	double y, z;
	y = (double)(rand() + 1) / (double)(RAND_MAX + 1);
	z = (double)log(y) * (double)(-1 / x);
	return z;
}

double getDistance(double coor_x, double coor_y) {	return sqrt(pow(coor_x, 2) + pow(coor_y, 2)); }	

int getCQI(UE *u)
{
	double distance = getDistance(u->coor_X, u->coor_Y);
	int CQI = 0;
	for (int i = 0; i < 15; i++)
	{
		if (distance <= LTECQIRange[i])
			CQI++;
		else
			break;
	}
	return CQI;
}

// int��string
string IntToString(int &i)
{
	string s;
	stringstream ss(s);
	ss << i;
	return ss.str();
}

int main()
{
	for (int i = 0; i < UEnumber; i++)
	{
		//Traffic request initial
		UEList[i].bit_rate = 10;
		UEList[i].packet_size = 800;
		if (i < UEnumber *0.33)
			UEList[i].delay_budget = 50;
		else
			if (i < UEnumber *0.66)
				UEList[i].delay_budget = 100;
			else
				UEList[i].delay_budget = 300;
		
		//Coordiante initial
		uniformdistribution(&UEList[i]);

		//Other calculation
		UEList[i].CQI = getCQI(&UEList[i]);
		UEList[i].lambdai = UEList[i].bit_rate / UEList[i].packet_size;
	}

	//give packet arrival time
	srand((unsigned)time(NULL));			//�üƺؤl
	string FileName;						//�ɮצW��
	fstream WriteFile;						//�ŧifstream����
	double BufferTimer = 0.0;				//�C��UE�beNB�̹���buffer���ɶ��b
	double InterArrivalTime = 0.0;			//packet��inter-arrival time
	int AcrossTTI = 0;						//�ΨӧP�_UE���ɶ��b�Apacket��inter-arrival time���L��L��TTI
	for (int i = 0; i < UEnumber; i++)
	{
		//cout << "UE" << i << endl;
		BufferTimer = 0.0;
		AcrossTTI = 0;
		string UEIndex = IntToString(i);
		FileName = "UE" + UEIndex + "_PAT.txt";				//PAT=packet arrival time
		WriteFile.open(FileName, ios::out | ios::trunc);	//�O���C��UE��PAT
		if (WriteFile.fail())
			cout << "�ɮ׵L�k�}��" << endl;
		else
		{
			for (int t = 0; t < simulation_time; t++)
			{
				int TTIPacketCount = 0;
				//�p��C��packet����Ӯɶ��I�A�ðO������TTI�C��UE��buffer�q
				while (BufferTimer <= t + 1)				//�Ψӭp�⦹TTI�ӤF�X��packet�M��TTI�����ɥثebuffer�̪���ƶq
				{
					WriteFile.setf(ios::fixed, ios::floatfield);
					WriteFile.precision(3);
					if (AcrossTTI)							//AcrossTTI = 1��inter arrival time����L��TTI; AcrossTTI=0���L
					{
						if (outputPAT == 1)
							WriteFile << BufferTimer << endl;	//�O���C��packet��arrival time
						//TTIPacketCount++;
					}						
					else
					{
						InterArrivalTime = exponentially_Distributed(UEList[i].lambdai);//�üƲ���inter-arrival time
						BufferTimer = BufferTimer + InterArrivalTime;                   //�����C��UE���ɶ��b
					}
					if (BufferTimer > t + 1)				//BufferTimer���L�W�L�ثe��TTI
					{
						AcrossTTI = 1;
						break;
					}
					else 
						if (AcrossTTI)
							AcrossTTI = 0;
						else
						{
							if (outputPAT == 1)
								WriteFile << BufferTimer << endl;	// �O���C��packet��arrival time
							//TTIPacketCount++;
						}						
					//cout << "Packet arrival time�G" << BufferTimer << endl;
				}
				//cout << "��" << t+1 << "��TTI��Packet�ơG" << TTIPacketCount << endl;
			}
		}
		WriteFile.close();
	}
	cout << "Give PAT end." << endl;

	//Ū���Ҧ�UE��PAT���Ȧs�_��
	string UEPacketPatternFileName;						//UE packet pattern���ɮצW��
	fstream ReadUEPAT;									//�ŧifstream����
	char UEPacketArrivalTime[200];						//�ΨӦ��stxt�C�@�檺���
	double ArrivalTime = 0.0;							//�ΨӦ��s��X�ӨC�@�檺���
	vector<double> TempPacketArrivalTime[UEnumber];		//�ΨӼȦsUE��packet pattern
	int NumUETemp = UEnumber;
	string NumUEIndex = IntToString(NumUETemp);
	for (int i = 0; i<UEnumber; i++)
	{
		string UEIndex = IntToString(i);
		UEPacketPatternFileName = "UE" + UEIndex + "_PAT.txt";
		ReadUEPAT.open(UEPacketPatternFileName, ios::in);
		if (!ReadUEPAT)
			cout << "�ɮ׵L�k�}��" << endl;
		else
		{
			while (ReadUEPAT >> UEPacketArrivalTime)
			{
				ArrivalTime = atof(UEPacketArrivalTime);
				TempPacketArrivalTime[i].push_back(ArrivalTime);
			}
		}
		ReadUEPAT.close();
	}


	//Debug
	for (int i = 0; i < UEnumber; i++)
		cout << UEList[i].bit_rate << " " << UEList[i].packet_size << " " << UEList[i].delay_budget << " " << UEList[i].coor_X << " " << UEList[i].coor_Y << " " << UEList[i].CQI << endl;
}