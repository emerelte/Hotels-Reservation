#ifndef HOTELSRESERVATION_HOTELS_H
#define HOTELSRESERVATION_HOTELS_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <exception>
#include <QDate>
#include <map>
#include <iterator>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::ifstream;
using std::ostream;
using std::setw;
using std::invalid_argument;
using std::multimap;
using std::pair;

class BadFileName : public invalid_argument{
public:
    BadFileName() : invalid_argument(string("Bad File name!")) {}
    virtual ~BadFileName();
};
class BadCityName : public invalid_argument{
public:
    BadCityName() : invalid_argument(string("Bad City Name!")) {}
    virtual ~BadCityName();
};
class BadHotelName : public invalid_argument{
public:
    BadHotelName() : invalid_argument(string("Bad Hotel Name")) {}
    virtual ~BadHotelName();
};
class BadRoomNumber : public invalid_argument{
public:
    BadRoomNumber() : invalid_argument(string("Bad Room Number")) {}
    virtual ~BadRoomNumber();
};
class NoRoomsAvailable : public invalid_argument{
public:
    NoRoomsAvailable() : invalid_argument(string("NoRoomsAvailable")) {}
    virtual ~NoRoomsAvailable();
};
class NoParent{
public:
    const char* what() const {return "No Parent";}
};

string toLower(string str);   //zwraca string z małych liter

class Room;//zapowiedź klasy dla wskaźnikowych składowych
class Reservation{
private:
    string name;
    string surname;
    QDate firstDay;
    QDate lastDay;
    Room * parent;//pokój, w którym dokonano rezerwacji
    Reservation & operator=(Reservation & rsv) = default; //operator przypisania nieużywany
    Reservation(Reservation & rsv) = default;             //konstruktor kopiujący nieużywany
public:
    Reservation(QDate m_firstDay, QDate m_lastDay,string m_name,string m_surname,Room * m_parent);
    Room & getParent() const {return *parent;}
    string toString();
    friend ostream & operator<<(ostream & os,const Reservation &rsv);//do wyświetlania
};

class Hotel;
class Room{
private:
    int number;
//    vector<Reservation *> reservations;
    multimap<string,Reservation &> reservations;
    Hotel * parent;
    vector<QDate> freeDates;
public:
    Room(int m_number, Hotel * m_parent);
    ~Room(){}
    Hotel & getParent(){return *parent;}
    vector<QDate> & getFreeDates(){return freeDates;}
    explicit operator int(){return number;}
    bool isFree(QDate firstDay,QDate lastDay);
    void book(QDate m_firstDay, QDate m_lastDay,string m_name, string m_surname);
    void addReservation(string surNameName,Reservation * rsv){reservations.insert(pair<std::string,Reservation &>(surNameName,*rsv));}
    bool hasReservations() const{
        return reservations.size();
    }   //czy są jakiekolwiek rezerwacje (do wyświetlania)
    void showReservations(ostream & os) const{
        for (auto it = reservations.begin(); it != reservations.end(); it++)
            os << (*it).second << std::endl;
    }
    vector<string> getReservations(string surNameName){
        vector<string> nameReservations;
        auto result = reservations.equal_range(toLower(surNameName));
        for (auto it = result.first; it != result.second; it++)
            nameReservations.push_back(it->second.toString());
        return nameReservations;
    }
};

class City;
class Hotel{
private:
    enum {howManyRooms=3};
    string name;
    double priceForNight;
    vector<Room *> rooms;
    City * parent; //miasto - rodzic, w którym jest hotel
public:
    Hotel(string m_name, int m_priceForNight, City * m_parent);
    ~Hotel(){
        for(auto it=rooms.begin();it!=rooms.end();++it){
            delete *it;
        }
    }
    void addRoom(Room * rom){rooms.push_back(rom);}
    explicit operator string(){
        return name;
    }//jawna konwersja na string - nazwę
    double getPrice() const {return priceForNight;}
    vector<Room *> getChildren(){
        return rooms;
    }
    const City & getParent() const {return *parent;}
    bool isFree(QDate m_firstDay, QDate m_lastDay) const {
        if (m_firstDay > m_lastDay)
            return false;
        for (auto it=rooms.begin();it!=rooms.end();++it){
            if ((*it)->isFree(m_firstDay,m_lastDay))
                return true;
        }
        return false;
    }
    Room & getRoom(QDate firstDay, QDate lastDay) const;
    Room & getRoom(int m_number) const;    //zwraca pokój na podstawie numeru
    void show(ostream & os) const;
    bool hasReservations() const{
        for (auto it = rooms.begin();it!=rooms.end();++it){
            if ((*it)->hasReservations())
                return true;
        }
        return false;
    }//czy są rezerwcaje
    void showReservations(ostream & os) const;//pokazuje rezerwacje
};

class DataBase;
//posiada wektor wskaźników na hotele - dzieci, może załadować dane z pliku tekstowego
class City{
private:
    string name;
    vector <Hotel *> hotels;
    DataBase * parent;
    bool getData(ifstream & inFile);//pobiera dane z pliku - pomaga konstruktorowi DataBase
public:
    City(string m_name, ifstream & inFile, DataBase * m_parent);
    City(string m_name){
        name = m_name;
        parent = nullptr;
    }//gdy tworzymy "niezależne" miasto (niezwiązane z plikiem)
    ~City(){
        for(auto it=hotels.begin();it!=hotels.end();++it){
            delete *it;
        }
    }//usuwa dzieci
    DataBase & getParent() const{
        if (parent==nullptr)
            throw NoParent();
        else
            return *parent;
    }
    vector<Hotel *> getChildren() {return hotels;}
    void addHotel(Hotel * htl){hotels.push_back(htl);}
    explicit operator string() const{return name;} //rzutowanie miasta na string (jego nazwę), tylko jawne
    Hotel & getHotel(string hotelName) const;
    void show(ostream & os) const;
    void showReservations(ostream & os) const;//pokazuje rezerwacje w mieście
};

//zawiera dane na temat wszstkich hoteli we wszystkich miastach
class DataBase{
private:
    ifstream inFile;
    string madeResName;
    std::ofstream madeReservations;
    vector <City *> cities;
public:
    DataBase(string fileName);
    ~DataBase(){
        for (auto it = cities.begin();it!=cities.end();++it)
            delete *it;
        inFile.close();
        madeReservations.close();
    }
    void addCity(City * cty){
        cities.push_back(cty);
    }
    vector<City *> getChildren(){return cities;}
    City & getCity(string m_city) const{
        for (auto it = cities.begin();it!=cities.end();++it){
            if (toLower(string((*(*it)))) == toLower(m_city)) {
                return *(*it);
            }
        }
        throw BadCityName();
    }
    vector<Hotel*> getHotels(string m_city){
        return getCity(m_city).getChildren();
    }
    void show(ostream & os) const {
        for (auto it = cities.begin();it!=cities.end();++it){
            (*it)->show(os);
        }
    }//pokazuje miasta, nazwy i ceny hoteli
    void showReservations(ostream & os) const {
        for (auto it = cities.begin();it!=cities.end();++it){
            (*it)->showReservations(os);
        }
    }//pokazuje tylko rezerwacje
    void writeToFile(){
        madeReservations.close();
        madeReservations.open(madeResName);
        showReservations(madeReservations);
    }
    vector<string> getReservations(string surNameName){
        vector<string> result;
        for (auto it0=cities.begin();it0!=cities.end();++it0){
            auto hotels=(*it0)->getChildren();
            for (auto it1=hotels.begin();it1!=hotels.end();++it1){
                auto rooms=(*it1)->getChildren();
                for (auto it2=rooms.begin();it2!=rooms.end();++it2){
                    vector<string> nRes = (*it2)->getReservations(surNameName);
                    result.insert(result.end(), nRes.begin(), nRes.end());
                }
            }
        }
        return result;
    }
};

#endif
