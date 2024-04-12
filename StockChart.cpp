//Created by Jakub Lewandowski

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <cfloat>

struct StockData {
    char date[11]; // YYYY-MM-DD
    double open;
    double high;
    double low;
    double close;
    double volume;
};

void clearScreen() {
#if defined(_WIN32)
    system("cls");
#else
    system("clear");
#endif
}

void displayMenu() {
    clearScreen();
    std::cout << "Jakub Lewandowski - 193142\n\n";
    std::cout << "w - Wykres\n";
    std::cout << "e - Edycja\n";
    std::cout << "q - Cofnij do Menu\n";
    std::cout << "x - Wyjscie\n";
}

StockData* loadCSV(const char* filename, int& dataSize) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << filename << std::endl;
        return nullptr;
    }

    int linesCount = 0;
    std::string line;
    while (std::getline(file, line)) ++linesCount;

    StockData* data = new StockData[linesCount - 1];
    //dataSize = linesCount - 1;
    //dataSize = 200; //Sztywna wartość
    file.clear();
    file.seekg(0, std::ios::beg);
    std::getline(file, line);

    int index = 0;
    char buffer[150];
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        ss.getline(data[index].date, 11, ',');
        ss.getline(buffer, 100, ','); data[index].open = atof(buffer);
        ss.getline(buffer, 100, ','); data[index].high = atof(buffer);
        ss.getline(buffer, 100, ','); data[index].low = atof(buffer);
        ss.getline(buffer, 100, ','); data[index].close = atof(buffer);
        ss.getline(buffer, 100, ','); data[index].volume = atof(buffer);
        index++;
    }

    file.close();
    return data;
}

void printValue(const StockData* stockData, int dataSize){
    for (int i = 0; i < dataSize; ++i) {
        std::cout << "Data: " << stockData[i].date
                  << ", Otwarcie: " << stockData[i].open
                  << ", Najwyzsza: " << stockData[i].high
                  << ", Najnizsza: " << stockData[i].low
                  << ", Zamkniecie: " << stockData[i].close
                  << ", Wolumen: " << std::fixed << std::setprecision(2) << stockData[i].volume << std::endl;
    }
}
//Maksymalna wartość do skalowania wykresu
double findMaxValue(const StockData* stockData, int dataSize) {
    double maxValue = 0.0;
    for (int i = 0; i < dataSize; ++i) {
        if (stockData[i].high > maxValue) {
            maxValue = stockData[i].high;
        }
    }
    return maxValue;
}

char** createChart(int height, int dataSize) {
    char** chart = new char*[height];
    for (int i = 0; i < height; ++i) {
        chart[i] = new char[dataSize + 1]; // +1 dla końca łańcuch
        std::fill_n(chart[i], dataSize, ' '); // Wypełnienie spacjami
        chart[i][dataSize] = '\0'; // Znak końca łańcucha
    }
    return chart;
}

void drawCandles(char** chart, const StockData* stockData, int dataSize, int height,
                 double maxValue) {
    for (int day = 0; day < dataSize; ++day) {
        const auto &sd = stockData[day];
        // Skalowanie wartości
        int scaledOpen = static_cast<int>((sd.open / maxValue) * height);
        int scaledClose = static_cast<int>((sd.close / maxValue) * height);
        int scaledLow = static_cast<int>((sd.low / maxValue) * height);
        int scaledHigh = static_cast<int>((sd.high / maxValue) * height);

        // Zapobieganie wyjściu poza zakres
        scaledOpen = std::min(height - 1, std::max
                (0, scaledOpen));
        scaledClose = std::min(height - 1, std::max(0, scaledClose));
        scaledLow = std::min(height - 1, std::max(0, scaledLow));
        scaledHigh = std::min(height - 1, std::max(0, scaledHigh));
        int Start = std::min(scaledLow, scaledHigh);
        int Stop = std::max(scaledLow, scaledHigh);
        char fillChar = (sd.close > sd.open) ? '#' : '0';
        for (int i = Start; i <= Stop; i++) {
            chart[height - 1 - i][day] = '|';
        }
        Start = std::min(scaledOpen, scaledClose);
        Stop = std::max(scaledOpen, scaledClose);
        for (int i = Start; i <= Stop; i++) {
            chart[height - 1 - i][day] = fillChar;
        }
    }
}

void printChart(char** chart, int height, int dataSize) {
    std::ofstream MyFile("Chart.txt");

    for (int i = 0; i < height; ++i) {

        int scaleLabel = height - 1 - i;
        std::cout << std::setw(3) << scaleLabel << " |";
        MyFile << std::setw(3) << scaleLabel << " |";

        for (int j = 0; j < dataSize; ++j) {
            MyFile << chart[i][j];
            std::cout << chart[i][j];
        }
        MyFile << std::endl;
        std::cout << std::endl;
    }

    MyFile.close();
}

int main() {
    const char* filename = "intc_us_data.csv";
    int dataSize = 200;
    int height = 50;
    double maxValue = 50;
    StockData* stockData = loadCSV(filename, dataSize);

    if (stockData == nullptr) {
        std::cerr << "Blad podczas ladowania danych." << std::endl;
        return -1;
    }

    char command = '\0';
    char** myChart = createChart(height, dataSize);

    while (command != 'q') {
        displayMenu();
        std::cin >> command;

        if (command == 'w') {
            clearScreen();
            drawCandles(myChart, stockData, dataSize, height, maxValue);
            printChart(myChart, height, dataSize);

            std::cout << "Nacisnij 'x', aby wrocic do menu...";
            while (std::cin.get() != 'x') { }
        } else if (command == 'e') {

            for (int i = 0; i < height; ++i) {
                delete[] myChart[i];
            }
            delete[] myChart;

            std::cout << "Podaj nowa ilosc dni (dataSize): ";
            std::cin >> dataSize;
            std::cout << "Podaj nowa wysokosc wykresu (height): ";
            std::cin >> height;

            delete[] stockData;
            stockData = loadCSV(filename, dataSize);
            if (stockData == nullptr) {
                std::cerr << "Blad podczas ladowania danych." << std::endl;
                return -1;
            }
            maxValue = findMaxValue(stockData, dataSize);

            myChart = createChart(height, dataSize);

            std::cout << "Nacisnij 'x', aby wrocic do menu...";
            while (std::cin.get() != 'x') { }
        }

        clearScreen();
    }

    for (int i = 0; i < height; ++i) {
        delete[] myChart[i];
    }
    delete[] myChart;

    if (stockData != nullptr) {
        delete[] stockData;
    }
    return 0;
}