#include "ellipsoid.h"

int main() {
    int i, j;
    double min_theta = 0;

    std::ofstream file("ellipsoid_data.txt");
    if (!file) {
        return 1;
    }

    // Генерация точек и сохранение в файл
    for (i = 0; i < n; i++) {
        double theta1 = M_PI * i / 2 / (n - 1);
        if (cos(theta1) < cos(min_theta))
            min_theta = theta1;

        double theta2 = M_PI * (i + 1) / 2 / (n - 1);
        if (cos(theta2) < cos(min_theta))
            min_theta = theta2;

        for (j = 0; j < 2 * n; j++) {
            double phi1 = 2 * M_PI * j / (2 * n);
            double phi2 = 2 * M_PI * (j + 1) / (2 * n);

            double x1 = a * sin(theta1) * cos(phi1);
            double y1 = b * sin(theta1) * sin(phi1);
            double z1 = c * cos(theta1);

            double x2 = a * sin(theta1) * cos(phi2);
            double y2 = b * sin(theta1) * sin(phi2);
            double z2 = c * cos(theta1);

            double x3 = a * sin(theta2) * cos(phi2);
            double y3 = b * sin(theta2) * sin(phi2);
            double z3 = c * cos(theta2);

            double x4 = a * sin(theta2) * cos(phi1);
            double y4 = b * sin(theta2) * sin(phi1);
            double z4 = c * cos(theta2);

            file << x1 << " " << y1 << " " << z1 << "\n";
            file << x2 << " " << y2 << " " << z2 << "\n";
            file << x3 << " " << y3 << " " << z3 << "\n";
            file << x4 << " " << y4 << " " << z4 << "\n";
        }
    }
    file.close();
    std::cout << "Данные сохранены в файл 'ellipsoid_data.txt'" << std::endl;

    std::ofstream file2("edge_data.txt");
    if (!file2) {
        return 1;
    }

    file2 << 0 << " " << 0 << " " << c * cos(min_theta) << "\n";

    for (i = 0; i < 2 * n; i++){
        double phi = 2.0f * M_PI * float(i) / float(2 * n);

        double x = a * sin(min_theta) * cos(phi);
        double y = b * sin(min_theta) * sin(phi);
        double z = c * cos(min_theta);

		file2 << x << " " << y << " " << z << "\n";
    }
    file2.close();
    std::cout << "Данные сохранены в файл 'edge_data.txt'" << std::endl;

    return 0;
}
