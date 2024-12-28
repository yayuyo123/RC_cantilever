#include <stdio.h>
#include<math.h>

// 鋼材 --------------------------------------------------------------------------
/**
 * @brief 引数に材料物性値とひずみを取り、応力を返す関数
 * 
 * @param Es ヤング率（弾性率）
 * @param yield 降伏応力
 * @param strain ひずみ
 * @return double 応力
 */
double steel(double Es, double yield, double strain) {
	const double HR = 0.01;  // 降伏後の剛性低下率

	// 応力計算
    double stress = Es * strain;

    if (fabs(stress) <= yield) {
        // 応力の絶対値が降伏応力以内の場合（弾性領域）
        return stress;
    } else {
        // 応力が降伏応力を超えた場合（塑性領域）
        if (strain > 0) {
            // ひずみが正の場合
            return HR * Es * strain + yield * (1 - HR);
        } else {
            // ひずみが負の場合
            return HR * Es * strain - yield * (1 - HR);
        }
    }
}

/**
 * @brief 応力-ひずみ曲線をCSV形式で出力する
 * 
 * @param filename 出力するCSVファイルの名前
 * @param Es ヤング率 (MPa)
 * @param yield 降伏応力 (MPa)
 * @param strain_min ひずみの最小値 (単位: マイクロ)
 * @param strain_max ひずみの最大値 (単位: マイクロ)
 * @param step ひずみのステップ幅 (単位: マイクロ)
 * @return int 成功なら0、失敗なら-1
 */
int output_steel_stress_strain_curve(
    const char *filename,
    double Es,
    double yield,
    int strain_min,
    int strain_max,
    int step
) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Failed to open file");
        return -1;
    }

    // CSVヘッダーを書き込む
    fprintf(fp, "Strain,Stress\n");

    // 指定範囲のひずみで応力を計算してCSVに出力
    for (int i = strain_min; i <= strain_max; i += step) {
        double strain = i * 0.000001; // マイクロひずみを変換
        double stress = steel(Es, yield, strain);
        fprintf(fp, "%lf,%lf\n", strain, stress);
    }

    fclose(fp);
    printf("The steel stress-strain curve has been output to '%s'.\n", filename);
    return 0;
}

// コンクリート ----------------------------------------------------------------
/**
 * @brief コンクリートの応力-ひずみ関係を計算する関数
 * 
 * 圧縮領域と引張領域で異なる応力-ひずみ関係を定義しています。
 * 圧縮領域では非線形挙動を考慮し、引張領域では線形挙動を考慮しています。
 * 
 * @param Ec コンクリートの引張弾性係数 (MPa)
 * @param Fc コンクリートの圧縮強度 (MPa)
 * @param Ft コンクリートの引張強度 (MPa)
 * @param strain ひずみ (単位: 無次元)
 * @return double 計算された応力 (MPa)
 */
double concrete(double Ec, double Fc, double Ft, double strain) {
    // 圧縮強度時のひずみ（定義されたピークひずみ）
    double peak = -0.0025;  // 圧縮領域のピークひずみ値

    if (strain <= 0.0) {
        // 圧縮領域: ひずみが負のとき (圧縮)
        if (strain >= peak) {
            // ひずみがピークに達していない場合、二次曲線を使用
            return Fc / pow(peak, 2) * pow((strain - peak), 2) - Fc;
        } else {
            // ひずみがピークに達した場合、一次線形応答を使用
            double stress = Fc / (3.0 * peak) * strain + 4.0 * Fc / 3.0;
            // 応力が負のときはそのまま、正のときは0を返す（圧縮強度の上限）
            return (stress < 0) ? stress : 0;
        }
    } else {
        // 引張領域: ひずみが正のとき (引張)
        if (Ec * strain <= Ft) {
            // 引張強度 Ft を超えない場合、線形応答
            return Ec * strain;
        } else {
            // 引張強度 Ft を超えると応力は0
            return 0;
        }
    }
}

/**
 * @brief 応力-ひずみ曲線をCSV形式で出力する
 * 
 * @param filename 出力するCSVファイルの名前
 * @param Ec コンクリートの引張弾性係数 (MPa)
 * @param Fc コンクリートの圧縮強度 (MPa)
 * @param Ft コンクリートの引張強度 (MPa)
 * @param strain_min ひずみの最小値 (単位: マイクロ)
 * @param strain_max ひずみの最大値 (単位: マイクロ)
 * @param step ひずみのステップ幅 (単位: マイクロ)
 * @return int 成功なら0、失敗なら-1
 */
int output_concrete_stress_strain_curve(
    const char *filename,
    double Ec,
    double Fc,
    double Ft,
    int strain_min,
    int strain_max,
    int step
) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Failed to open file");
        return -1;
    }

    // CSVヘッダーを書き込む
    fprintf(fp, "Strain,Stress\n");

    // 指定範囲のひずみで応力を計算してCSVに出力
    for (int i = strain_min; i <= strain_max; i += step) {
        double strain = i * 0.000001; // マイクロひずみを変換
        double stress = concrete(Ec, Fc, Ft, strain);
        fprintf(fp, "%lf,%lf\n", strain, stress);
    }

    fclose(fp);
    printf("The concrete stress-strain curve has been output to '%s'.\n", filename);
    return 0;
}