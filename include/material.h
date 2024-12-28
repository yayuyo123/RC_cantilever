#ifndef MATERIAL_H
#define MATERIAL_H

double steel(double Es, double yield, double strain);

int output_steel_stress_strain_curve(const char *filename, double Es, double yield, int strain_min, int strain_max, int step);

double concrete(double Ec, double Fc, double Ft, double strain);

int output_concrete_stress_strain_curve(
    const char *filename,
    double Ec,
    double Fc,
    double Ft,
    int strain_min,
    int strain_max,
    int step
);

#endif