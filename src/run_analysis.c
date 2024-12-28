#include <stdio.h>
#include <math.h>
#include "material.h"

#define ROWS 1000


//コンクリート断面の合力
double sumC(int num, double xn, double d, double dA, double Ec, double phi){
	double F = 0;
	for(int i = 0; i < num; i++){
		F = F + Con(Ec, phi * (d * (i + 0.5) - xn)) * dA;
	}
	return F;
}

//中立軸が間用コンクリート断面の合力
double sumC2(int num, double xn, double d, double b, double dA, double Ec, double phi){
	double F = 0;
	for(int i = 0; i < num; i++){
		if(xn > d * i && xn < d * (i + 1)){
			F += Con(Ec, phi * ((d * i + xn) / 2 - xn)) * (xn - d * i) * b + Con(Ec, phi * ((d * (i + 1) + xn) / 2 - xn)) * (d * (i + 1) - xn) * b;
		}else{
			F += Con(Ec, phi * (d * (i + 0.5) - xn)) * dA;
		}
	}
	return F;
}


//コンクリート断面の曲げモーメント
double MomentC(int num, double xn, double d, double dA, double Ec, double phi){
	double F = 0;
	for(int i = 0; i < num; i++){
		F = F + fabs(Con(Ec, phi * (d * (i + 0.5) - xn)) * dA) * (d * (i + 0.5));
	}
	return F;
}

//中立軸が間用コンクリート断面の曲げモーメント
double MomentC2(int num, double xn, double d, double b, double dA, double Ec, double phi){
	double F = 0;
	for(int i = 0; i < num; i++){
		if(xn > d * i && xn < d * (i + 1)){
			F += fabs(Con(Ec, phi * ((d * i + xn) / 2 - xn)) * (xn - d * i) * b) * ((d * i + xn) / 2 - xn );
			F += fabs(Con(Ec, phi * ((d * (i + 1) + xn) / 2 - xn)) * (d * (i + 1) - xn) * b) * ((d * (i + 1) + xn) / 2 - xn);
		}else{
			F += fabs(Con(Ec, phi * (d * (i + 0.5) - xn)) * dA) * (d * (i + 0.5) - xn);
		}
	}
	return F;
}

// 断面解析

// 部材解析

int main(void){
	char line[256];
	double Ec, Es;
	double b, D;
	double Au, Ad;
	double Nu, Nd;
	double l1, l2;
	
	//データ読込み
	FILE *fr = fopen("InputDate.txt", "r");
	if(fr == NULL){
		printf("File cant be opend.\n");
	}else{
		if(fgets(line, sizeof(line), fr) != NULL){
			if(sscanf(line, "%lf %lf", &Ec, &Es) != 2){
				printf("1 Errer\n");
				fclose(fr);
				return 1;
			}
		}else{
			printf("1 non\n");
			fclose(fr);
			return 1;
		}
		
		if(fgets(line, sizeof(line), fr) != NULL){
			if(sscanf(line, "%lf %lf", &b, &D) != 2){
				printf("2 Errer\n");
				fclose(fr);
				return 1;
			}
		}else{
			printf("2 non\n");
			fclose(fr);
			return 1;
		}
		
		if(fgets(line, sizeof(line), fr) != NULL){
			if(sscanf(line, "%lf %lf", &Au, &Ad) != 2){
				printf("3 Errer\n");
				fclose(fr);
				return 1;
			}
		}else{
			printf("3 non\n");
			fclose(fr);
			return 1;
		}
		
		if(fgets(line, sizeof(line), fr) != NULL){
			if(sscanf(line, "%lf %lf", &Nu, &Nd) != 2){
				printf("4 Errer\n");
				fclose(fr);
				return 1;
			}
		}else{
			printf("4 non\n");
			fclose(fr);
			return 1;
		}
		
		if(fgets(line, sizeof(line), fr) != NULL){
			if(sscanf(line, "%lf %lf", &l1, &l2) != 2){
				printf("5 Errer\n");
				fclose(fr);
				return 1;
			}
		}else{
			printf("5 non\n");
			fclose(fr);
			return 1;
		}
	}
	
	fclose(fr);
		
	//断面解析 m-phi
	double Section[ROWS][5] = {{0}};
	double Convergent[ROWS][4] = {{0}};
	double SUM, sum, M;
	int N = 26; //分割数
	double d = D / N;
	double dA = d * b;
	int n = N / 2;//Nが奇数の場合の処理
	double xn = d * n;
	int Cycle = 0;
	
	double tol = 0.1; //Tolerance 許容範囲
	int MAX = 200; //回数
	
	int k = 0;
	int Index = 56;
	double rate = 0.5;
	
	for(double phi = 0; phi < 0.00001; phi += 0.00000005){
		SUM = sumC(N, xn, d, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu + Ste(Es, phi * (l2 - xn)) * Ad * Nd;
		sum = SUM;
		Cycle = 0;
		
		//printf("SUM = %lf\n", SUM);
		//printf("sum = %lf\n", sum);
		if(fabs(SUM) > 0.0000001){
			while(fabs(SUM) > 0.0000001){
				if(SUM > 0 != sum > 0){
					//収束計算
					double Delt = -d / 2;
					for(int i = 0; i < MAX; i++){
						xn += Delt; 
						sum = SUM;
						SUM = sumC2(N, xn, d, b, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu+ Ste(Es, phi * (l2 - xn)) * Ad * Nd;
						Cycle += 1;
						if(k == Index){
							Convergent[i][0] = i + 1;
							Convergent[i][1] = Delt;
							Convergent[i][2] = xn;
							Convergent[i][3] = SUM;
						}
						if(fabs(SUM) < tol){
							M = MomentC2(N, xn, d, b, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu * (l1 - xn) + Ste(Es, phi * (l2 - xn)) * Ad * Nd * (l2 - xn);
							break;
						}else{
							if(SUM > 0 == sum > 0){
								if(fabs(SUM) - fabs(sum) >= 0){
									Delt *= -1 * rate;
								}else{
								}
							}else{
								Delt *= -1 * rate;
							}
						}
					}
					M = MomentC2(N, xn, d, b, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu * (l1 - xn) + Ste(Es, phi * (l2 - xn)) * Ad * Nd * (l2 - xn);
					break;
				}else{
					if(SUM > 0){
						xn += d;
						sum = SUM;
						SUM = sumC(N, xn, d, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu + Ste(Es, phi * (l2 - xn)) * Ad * Nd;
						M = MomentC(N, xn, d, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu * (l1 - xn) + Ste(Es, phi * (l2 - xn)) * Ad * Nd * (l2 - xn);
					}else{
						xn -= d;
						sum = SUM;
						SUM = sumC(N, xn, d, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu+ Ste(Es, phi * (l2 - xn)) * Ad * Nd;
						M = MomentC(N, xn, d, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu * (l1 - xn) + Ste(Es, phi * (l2 - xn)) * Ad * Nd * (l2 - xn);
					}
				}
			}
		}else{
			M = MomentC(N, xn, d, dA, Ec, phi) + Ste(Es, phi * (l1 - xn)) * Au * Nu * (l1 - xn) + Ste(Es, phi * (l2 - xn)) * Ad * Nd * (l2 - xn);
		}
		Section[k][0] = phi;
		Section[k][1] = M;
		Section[k][2] = xn;
		Section[k][3] = SUM;
		Section[k][4] = Cycle;
		k += 1;
	}
	printf("[Calculation]Section --OK\n");
	
	
	
	//スパン方向 荷重-変形
	double StepDate[ROWS][3] = {{0}};
	double FixMo = 1000000; //固定端モーメント
	
	int Step = 10;
	
	//材軸方向の計算点
	double Length = 2000; //材長
	int Num = 20; //材軸方向の分割数
	double L = Length / (double)Num;
	for(int i = 0;i <= Num;i++){
		StepDate[i][0] = L * (double)i;
		StepDate[i][1] = (double)i / (double)Num * FixMo;
	}
	
	
	
	
	// ConcreteCurveテキストファイルへの書き込み
    FILE *fw1 = fopen("ConcreteCurve.txt", "w");
    if (fw1 == NULL) {
        fprintf(stderr, "[Error]ConcreteCurve.txt\n");
        return 1;
    }
    for (double stra = -0.01; stra <= 0.002; stra += 0.0001) {
		fprintf(fw1, "%10.5lf", stra);
		fprintf(fw1, " ");
    	fprintf(fw1, "%10.5lf", Con(Ec, stra));
		fprintf(fw1, "\n");
    }
    // ファイルを閉じる
    fclose(fw1);
	printf("[write]ConcreteCurve.txt ---OK\n");
	
	
	// SteelCurveテキストファイルへの書き込み
    FILE *fw2 = fopen("SteelCurve.txt", "w");
    if (fw2 == NULL) {
        fprintf(stderr, "[Error]SteelCurve.txt\n");
        return 1;
    }
    for (double stra = -0.5; stra <= 0.51; stra += 0.01) {
		fprintf(fw2, "%10.3lf", stra);
		fprintf(fw2, " ");
    	fprintf(fw2, "%10.3lf", Ste(Es, stra));
		fprintf(fw2, "\n");
    }
    // ファイルを閉じる
    fclose(fw2);
	printf("[write]SteelCurve.txt ---OK\n");
	
	
	// Convergentテキストファイルへの書き込み
    FILE *fw3 = fopen("Convergent.txt", "w");
    if (fw3 == NULL) {
        fprintf(stderr, "[Error]Convergent.txt\n");
        return 1;
    }
	fprintf(fw3, "#Index=%d\n", Index);
    for (int i = 0; i < MAX; ++i) {
    	if(i > 0){
    		if(Convergent[i][0] < 0.000001){
    		break;
    		}
    	}
        for (int j = 0; j < 4; ++j) {
            fprintf(fw3, "%17.5lf", Convergent[i][j]);
            // 最後の要素以外にはスペースを追加
            if (j < 3) {
                fprintf(fw3, " ");
            }
        }
        // 行の終わりに改行を追加
        fprintf(fw3, "\n");
    }
	printf("[write]BinarySearch.txt ---OK\n");
	
    // ファイルを閉じる
    fclose(fw2);
	
	
    // Sectionファイルへの書き込み
    FILE *fw4 = fopen("Section.txt", "w");
    if (fw4 == NULL) {
        fprintf(stderr, "[Error]Section.txt\n");
        return 1;
    }
	fprintf(fw4, "#Index      Phi[-]   Moment[Nmm]   xn[mm]     Sum[N]  Cycle");
	fprintf(fw4, "\n");
    for (int i = 0; i < ROWS; ++i) {
    	if(i > 0){
    		if(Section[i][0] < 0.0000000001){
    		break;
    		}
    	}
    	fprintf(fw4, "%6d", i);
    	fprintf(fw4, "  ");
    	fprintf(fw4, "%.8lf", Section[i][0]);
    	fprintf(fw4, "  ");
    	fprintf(fw4, "%12.2lf", Section[i][1]);
    	fprintf(fw4, "  ");
    	fprintf(fw4, "%7.3lf", Section[i][2]);
    	fprintf(fw4, "  ");
    	fprintf(fw4, "%9.3lf", Section[i][3]);
        fprintf(fw4, "  ");
    	fprintf(fw4, "%5.0lf", Section[i][4]);
        fprintf(fw4, "\n");
    }
    // ファイルを閉じる
    fclose(fw4);
	printf("[write]Section.txt ---OK\n");
	
	// StepDate.txtへの書き込み
    FILE *fw5 = fopen("StepDate.txt", "w");
    if (fw5 == NULL) {
        fprintf(stderr, "[Error]StepDate.txt\n");
        return 1;
    }
	fprintf(fw5, "#STEP=%d\n", Step);
	fprintf(fw5, "#Cordinate[mm]    Moment[Nmm]   Phi[]");
	fprintf(fw5, "\n");
    for (int i = 0; i <= Num; ++i) {
    	if(i > 0){
    		if(StepDate[i][0] < 0.000001){
    		break;
    		}
    	}
        for (int j = 0; j < 3; ++j) {
            fprintf(fw5, "%14.2lf", StepDate[i][j]);
            // 最後の要素以外にはスペースを追加
            if (j < 2) {
                fprintf(fw5, " ");
            }
        }
        // 行の終わりに改行を追加
        fprintf(fw5, "\n");
    }
	printf("[write]StepDate.txt ---OK\n");
	
    // ファイルを閉じる
    fclose(fw5);
	
    printf("[END]this program\n");
		
	return 0;
}

