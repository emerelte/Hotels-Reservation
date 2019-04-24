#include "../headers/hotelview.h"
#include "../headers/hotels.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <exception>
#include <QDebug>
#include <QtWidgets>

int main(int argc, char * argv[]) {
    QApplication app(argc,argv);
    HotelView inFrm;
    inFrm.show();
    return app.exec();
    /*
    using std::string;using std::cout;using std::cin;using std::endl;using std::ifstream;
    string fileName = "/home/maciej/Dokumenty/JPO/HotelsReservation/spis_hoteli.txt";
    try {
        DataBase allData(fileName);
        allData.show();
        cout << "Podaj miasto, \"KONIEC\" kończy:\n";
        string cityName;
        while (getline(std::cin, cityName) && toLower(cityName) != "koniec") {
            try {
                (allData.getCity(cityName)).show();
                cout << "Wybierz hotel. \"KONIEC\" kończy:\n";
                string hotelName;
                while (getline(std::cin, hotelName) && toLower(hotelName) != "koniec") {
                    try {
                        (allData.getCity(cityName).getHotel(hotelName)).show();
                        QDate d1 = QDate::currentDate();
                        QDate d2 = QDate::currentDate().addDays(5);
                        cout << "Podaj dane rezerwacji (imię i nazwisko)\n";
                        string name, surname;
                        cin >> name >> surname;
                        while (std::cin.get() !=
                               '\n')//trzeba wyczyścić bufor, bo inaczej bierze nazwisko za nazwę hotelu
                            continue;
                        cin.clear();
                        try {
                            (allData.getCity(cityName).getHotel(hotelName)).getRoom(d1, d2).
                                    book(d1, d2.addDays(5), name, surname);
                        } catch (const NoRoomsAvailable &e) {
                            cout << e.what();
                        }
                    } catch (const invalid_argument &e) {
                        cout << e.what() << endl;
                    }
                    cout << "Wybierz hotel. \"KONIEC\" kończy:\n";
                }
            } catch (const invalid_argument & e) {
                cout << e.what() << endl;
            }
            cout << "Podaj miasto, \"KONIEC\" kończy:\n";
        }
        allData.showReservations();
    } catch (const std::invalid_argument & e){
        cout << e.what();
    }

    return 0;
    */
}
