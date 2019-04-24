#ifndef HOTELSRESERVATION_HOTELVIEV_H
#define HOTELSRESERVATION_HOTELVIEV_

#include "hotels.h"
#include "resultsview.h"
#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QObject>
#include <QLabel>
#include <fstream>

class QLineEdit;
class QDateEdit;
class QPushButton;
class QDialogButtonBox;
class QComboBox;

class HotelView : public QDialog {
    Q_OBJECT
public:
    explicit HotelView(QWidget* parent = nullptr);
protected slots:
    void accept();
private:
    DataBase * allData;

    //do wpisywania
    QLabel* picture;
    QComboBox* city;
    QComboBox* hotel;
    QLineEdit* name;
    QLineEdit* surname;
    QDateEdit* firstDay;
    QDateEdit* lastDay;
    //do wyświetlania
    QLabel* price;
    QLabel* isFree;
    //zapisywanie/odrzucanie
    QDialogButtonBox* buttons;
    void initView();
    void refreshHotel();
    void refreshPicture();
    void refreshPrice();
};


#endif //HOTELSRESERVATION_HOTELVIEV_H
