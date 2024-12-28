#include <stdio.h>
#include <assert.h>
#include "material.h"

void test_steel() {
    double Es = 210000; // ヤング率 [MPa]
    double yield = 295; // 降伏応力 [MPa]

    // テストケース1: 弾性領域
    double strain1 = 0.001; // 弾性領域のひずみ
    assert(steel(Es, yield, strain1) == Es * strain1);

    // テストケース2: 降伏応力ちょうど
    double strain2 = yield / Es; // 降伏応力に対応するひずみ
    assert(steel(Es, yield, strain2) == yield);

    // テストケース3: 降伏応力を超えた塑性領域
    double strain3 = 0.002; // 降伏応力を超えたひずみ
    double expected_stress3 = 0.01 * Es * strain3 + yield * (1 - 0.01);
    assert(steel(Es, yield, strain3) == expected_stress3);

    // テストケース4: 負のひずみ（圧縮）
    double strain4 = -0.002; // 降伏応力を超えた負のひずみ
    double expected_stress4 = 0.01 * Es * strain4 - yield * (1 - 0.01);
    assert(steel(Es, yield, strain4) == expected_stress4);

    // テストケース5: ひずみがゼロ  
    assert(steel(Es, yield, 0.0) == 0.0);
    printf("All tests for the steel function passed successfully.\n");

   // テストケース: CSV出力
   output_steel_stress_strain_curve("tests/output_steel_stress_strain_curve.csv", Es, yield, -3000, 3000, 10);
}

void test_concrete() {
    // 引張弾性係数 Ec, 圧縮強度 Fc, 引張強度 Ft
    double Ec = 20000;  // ヤング係数 (MPa)
    double Fc = 40;     // 圧縮強度 (MPa)
    double Ft = 3;      // 引張強度 (MPa)

    // 圧縮領域テスト: ひずみが負のとき
    double strain_compression = -0.001;
    double stress_compression = concrete(Ec, Fc, Ft, strain_compression);
    // 圧縮強度 Fc と比較して応力が正しく計算されているか確認
    assert(stress_compression >= -1 * Fc);

    // 圧縮領域のピーク時テスト
    double strain_peak = -0.0025; // 圧縮領域のピーク値
    double stress_peak = concrete(Ec, Fc, Ft, strain_peak);
    // 応力がピーク値で計算されているか確認
    assert(stress_peak == -1 * Fc);

    // 引張領域テスト: ひずみが正のとき
    double strain_tension = Ft / Ec - 0.0001;
    double stress_tension = concrete(Ec, Fc, Ft, strain_tension);
    // 引張強度 Ft と比較して応力が計算されているか確認
    assert(stress_tension == Ec * strain_tension);

    // 引張領域で引張強度を超えたテスト
    double strain_tension_limit = 0.002;  // 引張強度 Ft を超えるひずみ
    double stress_tension_limit = concrete(Ec, Fc, Ft, strain_tension_limit);
    // 引張強度 Ft を超えた場合は応力が0になることを確認
    assert(stress_tension_limit == 0);

    printf("All concrete function tests passed successfully.\n");

    // テストケース: CSV出力
    output_concrete_stress_strain_curve(
        "tests/output_concrete_stress_strain_curve.csv",
        Ec,
        Fc,
        Ft,
        -3000,
        3000,
        10
    );
}

int main() {
    test_steel();
    test_concrete();
    return 0;
}