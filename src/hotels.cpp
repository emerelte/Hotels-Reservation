#include "../headers/hotels.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <exception>
#include <sstream>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::ifstream;
using std::ostream;
using std::for_each;
using std::setw;

BadFileName::~BadFileName(){}
BadCityName::~BadCityName(){}
BadHotelName::~BadHotelName(){}
BadRoomNumber::~BadRoomNumber(){}
NoRoomsAvailable::~NoRoomsAvailable(){}

string toLower(string str){
    for_each(str.begin(),str.end(),[](char & ch){ch = static_cast<char>(tolower(ch));});
    return str;
}//zamiana stringa na małe litery, bez ingerencji w niego - działa na kopii

Reservation::Reservation(QDate m_firstDay, QDate m_lastDay,string m_name, string m_surname, Room *m_parent) {
    name = m_name;
    surname = m_surname;
    parent = m_parent;
    firstDay = m_firstDay;
    lastDay = m_lastDay;
    auto it1 = (parent->getFreeDates()).begin();//u rodzica pozostaje mniej wolnych dat
    while (*it1 < firstDay)
        ++it1;
    auto it2 = it1;
    while (*it2 <= lastDay)
        ++it2;
    (parent->getFreeDates()).erase(it1, it2);
    string surNameName=toLower(surname+name);
    parent->addReservation(surNameName,this);
}
string Reservation::toString(){
    string result;
    result+=string(getParent().getParent().getParent()) + "~";//miasto
    result+=string(getParent().getParent()) + "~";//hotel
    result+=firstDay.toString("dd/MM/yy").toStdString() + "~";//pierwszy dzień
    result+=lastDay.toString("dd/MM/yy").toStdString() + "~";//ostatni dzień
    result+=name + "~";
    result+=surname + "~";
    std::stringstream ss;
    ss << (int(getParent()));
    result+=ss.str();
    return result;
}
ostream & operator<<(ostream & os,const Reservation & rsv){
    os << string(rsv.getParent().getParent().getParent()) << "~";//miasto
    os << string(rsv.getParent().getParent()) << "~";//hotel
    os << rsv.firstDay.toString("dd/MM/yy").toStdString() << "~";//pierwszy dzień
    os << rsv.lastDay.toString("dd/MM/yy").toStdString() << "~";//ostatni dzień
    os << rsv.name << "~";
    os << rsv.surname << "~";
    std::stringstream ss;
    ss << (int(rsv.getParent()));
    os << ss.str() << std::endl;
    //os << std::right << setw(27) << rsv.name + string(" ") + rsv.surname << endl;
    return os;
}

Room::Room(int m_number, Hotel * m_parent) {
    number = m_number;
    parent = m_parent;
    parent->addRoom(this);
    QDate beginDate = QDate::currentDate();//inicjalizacja freeDays - 30 dni od dziś
    QDate endDate = beginDate.addDays(30);
    while (beginDate < endDate){
        freeDates.push_back(beginDate);
        beginDate = beginDate.addDays(1);
    }
}
bool Room::isFree(QDate firstDay, QDate lastDay) {
    if (firstDay > lastDay)
        return false;
    auto it1 = std::find (freeDates.begin(), freeDates.end(), firstDay);//sprawdza, czy
    auto it2 = std::find (freeDates.begin(), freeDates.end(), lastDay); //pomiędzy datami
    if (it1==freeDates.end() || it2==freeDates.end())
        return false;
    return distance(it1, it2) == (lastDay.toJulianDay() - firstDay.toJulianDay());
}
void Room::book(QDate m_firstDay, QDate m_lastDay,string m_name, string m_surname) {
    new Reservation(m_firstDay,m_lastDay,m_name,m_surname,this);
    getParent().getParent().getParent().writeToFile();
}

Hotel::Hotel(string m_name, int m_priceForNight, City *m_parent) {
    name = m_name;
    priceForNight = m_priceForNight;
    parent = m_parent;
    parent->addHotel(this);
    for (int i = 0; i < howManyRooms; ++i) {
        new Room((i + 1), this);
    }
}
Room & Hotel::getRoom(QDate firstDay, QDate lastDay) const {
    for (auto it=rooms.begin();it!=rooms.end();++it){
        if ((*it)->isFree(firstDay,lastDay))
            return *(*it);
    }
    throw NoRoomsAvailable();
}
Room & Hotel::getRoom(int m_number) const {
    if (m_number < 1 || m_number > howManyRooms)
        throw BadRoomNumber();
    return *(rooms[static_cast<std::vector<Room *>::size_type>(m_number-1)]);
}
void Hotel::show(ostream & os) const{
    os << setw(25) << std::left << name << " " << std::right << setw(5) << priceForNight << endl;
}
void Hotel::showReservations(ostream & os) const{
    for (auto it = rooms.begin();it!=rooms.end();++it)
        (*it)->showReservations(os);
}

//dodaje miasto o m_name z pliku inFile, przyporządkowuje do m_parent
City::City(string m_name, ifstream & inFile, DataBase * m_parent){
     name = m_name;
     getData(inFile);
     parent = m_parent;
     m_parent->addCity(this);
}//gdy pobieramy z pliku
bool City::getData(std::ifstream & inFile) {
    std::string hotelName;std::string hotelPrice;
    std::string hotel = "Hotel";
    std::streampos oldpos;
    while(getline(inFile,hotelName) && hotelName.substr(0,hotel.size())==hotel){
        hotelPrice = hotelName.substr(hotelName.find('(')+1,
                hotelName.find(')')-(hotelName.find('(')+1)-std::string("zl/noc").size());
        hotelName = hotelName.substr(hotel.size()+1,hotelName.find('(')-(hotel.size()+2));
        new Hotel(hotelName,std::stoi(hotelPrice),this);
        oldpos = inFile.tellg();  // stores the position
    }
    inFile.seekg (oldpos);    // get back to the position
    return true;
}
Hotel & City::getHotel(std::string hotelName) const{
    for (auto it = hotels.begin();it!=hotels.end();++it){
        if (toLower(std::string(*(*it)))==toLower(hotelName)){
            return *(*it);
        }
    }
    throw BadHotelName();
}
void City::show(ostream & os) const {
    os << "Miasto: " << name << std::endl;
    for (auto it = hotels.begin();it!=hotels.end();++it){
        (*it)->show(os);
    }
}
void City::showReservations(ostream & os) const {
    for (auto it = hotels.begin();it!=hotels.end();++it){
        if ((*it)->hasReservations()){
            //os << name << std::endl;
            while (it!=hotels.end()){
                (*it)->showReservations(os);
                ++it;
            }
            break;
        }
    }
}

//tworzy bazę danych z pliku fileName, zapisuje rezerwacje do pliku resetvations.txt. Nadpisuje ten plik (trunc)
DataBase::DataBase(string fileName) : inFile(fileName), madeResName("reservations.txt"),madeReservations(madeResName, std::ios::trunc){
    if (!inFile.is_open()){
        throw BadFileName();
    }
    if(!madeReservations.is_open()){
        throw BadFileName();
    }

    string cityName;
    string hotel = "Hotel"; string koniec = "koniec";
    while (getline(inFile,cityName)){
        cityName = cityName.substr(0,cityName.find(char(13)));//bo Carriage Return
        if (cityName.substr(0,koniec.size()) == koniec)
            break;
        if (!(cityName.substr(0,hotel.size())==hotel)){//gdy linia nie jest nazwą hotelu, tylko miasta
            new City(cityName,inFile,this);
        }
    }
}
