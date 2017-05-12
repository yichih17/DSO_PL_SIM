#ifndef _DEFIEN_H
#define _DEFIEN_H

//Scenario patameter
#define power_eNB 46.0			//dBm
#define radius_eNB 1723			//m
#define total_RBG 100			//Number of total RBG (20Mhz=100)
#define	total_symbol 14			//Number of symbol in a RB (14)
#define ctrl_symbol 2			//Number of symbol for control signal in a RB (1~3)
#define subcarrier 12			//Number of subcarrier (12)
#define resource_element (total_symbol - ctrl_symbol) * subcarrier	//Number of resource element per RB for data

#define power_AP  23.0			//dBm

#define rho_max 0.99			//Max load of BSs

//Algoritm parameter
#define depth_max 2
#define satisfied_TH 75

//Simulation parameter
#define simulation_time 100000	//ms(TTI
#define UE_dis_mode 1			//0: uniform 1:hotspot
#define UE_type_number 3		//DB = 50, 100, 300ms

#include<vector>

enum type_bs { macro, ap, ue };
enum type_ue { type1, type2, type3, type4 };
enum type_distribution { uniform, hotspot };

struct BS;
struct UE;
extern std::vector <BS> vbslist;
extern std::vector <UE> vuelist;

int LTECQIRange[];
int range_ap[];
extern double macro_eff[15];
extern double ap_capacity[8];

/*attribute of BS*/
struct BS
{
	int num;
	type_bs type;
	double coor_X, coor_Y;
	double lambda;
	double systemT;
	double systemT_constraint;
	std::vector <UE*> connectingUE;
};

/*attribute of UE*/
struct UE
{
	int num;
	type_ue type;
	double coor_X, coor_Y;
	double lambdai;
	int delay_budget;
	double packet_size;
	double bit_rate;
	BS* connecting_BS;
	int CQI;
	std::vector <BS*> availBS;
};

struct result
{
	int outage_number;
};

struct connection_status
{
	std::vector <BS> bslist;
	std::vector <UE> uelist;
	int influence;
	int outage_dso;
};

struct hotspot
{
	double coor_x;
	double coor_y;
};


#endif // !_DEFIEN_H