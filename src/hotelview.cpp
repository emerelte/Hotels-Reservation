#include "../headers/hotelview.h"
#include "../headers/resultsview.h"

#include <QFormLayout>
#include <QDateEdit>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <iostream>
#include <QComboBox>
#include <QImage>
#include <QLabel>
#include <QPixmap>

using std::cout;
using std::string;

HotelView::HotelView(QWidget* parent) : QDialog(parent) {
    allData = new DataBase(string("spis_hoteli.txt"));
    setWindowTitle("Rezerwacja Hoteli");
    initView();
}
void HotelView::initView() {
    city = new QComboBox;
    vector<City *> allCities = allData->getChildren();
    for (auto it=allCities.begin();it!=allCities.end();++it){
        city->addItem(QString::fromStdString(string(*(*it))));
    }
    hotel = new QComboBox;
    vector<Hotel *> allHotels = allData->getHotels(city->currentText().toStdString());
    for (auto it=allHotels.begin();it!=allHotels.end();++it){
        hotel->addItem(QString::fromStdString(string(*(*it))));
    }
    name = new QLineEdit;
    surname = new QLineEdit;
    firstDay = new QDateEdit;
    firstDay->setDisplayFormat("dd/MM/yyyy");
    firstDay->setDate(QDate::currentDate());
    lastDay = new QDateEdit;
    lastDay->setDisplayFormat("dd/MM/yyyy");
    lastDay->setDate(QDate::currentDate());
    price = new QLabel();
    refreshPrice();
    isFree = new QLabel;
    buttons = new QDialogButtonBox(QDialogButtonBox::Save |
                                   QDialogButtonBox::Cancel);
    QVBoxLayout* vbox = new QVBoxLayout;
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Miasto"), city);                               /* Stwórz i dodaj etykietę QLabel oraz widżet do wprowadzania danych — wszystko w jednej linii! */
    layout->addRow(tr("Hotel"), hotel);                               /* Stwórz i dodaj etykietę QLabel oraz widżet do wprowadzania danych — wszystko w jednej linii! */
    layout->addRow(tr("Imię"), name);                               /* Stwórz i dodaj etykietę QLabel oraz widżet do wprowadzania danych — wszystko w jednej linii! */
    layout->addRow(tr("Nazwisko"), surname);                               /* Stwórz i dodaj etykietę QLabel oraz widżet do wprowadzania danych — wszystko w jednej linii! */
    layout->addRow(tr("Pierwszy dzień"), firstDay);
    layout->addRow(tr("Ostatni dzień"), lastDay);
    layout->addRow(tr("Cena"),price);

    picture = new QLabel();
    refreshPicture();
    vbox->addWidget(picture);
    vbox->addLayout(layout);                /* W ten sposób możemy osadzić jeden układ w drugim. */
    vbox->addWidget(buttons);

    Q_ASSERT(vbox->parent() == nullptr);
    setLayout(vbox);                        /* Zmienia rodzica już wcześniej rozmieszczonych widżetów. */
    Q_ASSERT(vbox->parent() == this);

    connect (city, &QComboBox::currentTextChanged,
             [this]()->void{
                string ctyName = city->currentText().toStdString();
                refreshHotel();
        });
    connect(hotel, &QComboBox::currentTextChanged,
            [this]()->void{
               refreshPicture();
               refreshPrice();
       });
    connect(firstDay, &QDateEdit::dateChanged,
            [this]()->void{
               refreshPrice();
       });
    connect(lastDay, &QDateEdit::dateChanged,
            [this]()->void{
               refreshPrice();
       });
    connect (buttons, SIGNAL(accepted()),
             this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()),
            this, SLOT(reject()));
}
void HotelView::refreshHotel() {
    disconnect(hotel,nullptr,nullptr,nullptr);
    hotel->clear();
    vector<Hotel *> allHotels = allData->getHotels(city->currentText().toStdString());
    for (auto it=allHotels.begin();it!=allHotels.end();++it){
        hotel->addItem(QString::fromStdString(string(*(*it))));
    }
    refreshPicture();
    refreshPrice();
    connect(hotel, &QComboBox::currentTextChanged,
            [this]()->void{
               refreshPicture();
               refreshPrice();
       });
}
void HotelView::refreshPicture(){
    string sysCommand = "./getPicture.sh ";//szybkie, marnej jakości
    sysCommand.append(city->currentText().toStdString());
    sysCommand.append(" hotel ");
    sysCommand.append(hotel->currentText().toStdString());
    system(sysCommand.c_str());
    QPixmap qpm = QPixmap("pictures/currentHotel");
    picture->setPixmap(QPixmap("pictures/currentHotel"));
    picture->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    int w = picture->width();
    int h = picture->height();
    picture->setPixmap(qpm.scaled(w*2/3,h*2/3,Qt::KeepAspectRatio));
}
void HotelView::refreshPrice(){
    double tempPrice = allData->getCity(city->currentText().toStdString()).getHotel(hotel->currentText().toStdString()).getPrice();
    tempPrice *= lastDay->date().toJulianDay()-firstDay->date().toJulianDay() + 1;
    if (!allData->getCity(city->currentText().toStdString()).getHotel(hotel->currentText().toStdString()).isFree(firstDay->date(),lastDay->date()))
        price->setText("Nie ma możliwości rezerwacji na tę datę.");
    else
    price->setText(QString::number(tempPrice));
}
void HotelView::accept() {
    string m_city = city->currentText().toStdString();
    string m_hotel = hotel->currentText().toStdString();
    string m_name = name->displayText().toStdString();
    string m_surname = surname->displayText().toStdString();
    QDate m_firstDay = firstDay->date();
    QDate m_lastDay = lastDay->date();
    try {
        allData->getCity(m_city).getHotel(m_hotel).getRoom(m_firstDay, m_lastDay).
            book(m_firstDay, m_lastDay, m_name, m_surname);
        string surnameName=m_surname+m_name;
        ResultsView* rv= new ResultsView(allData,surnameName);
        (*rv).exec();
        delete rv;
        int answer = QMessageBox::question(this, tr("Udana rezerwacja"),
                tr("Dokonamy kolejnej rezerwacji?"),QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::No)
            QDialog::accept();
        refreshPrice();//odświeża cenę, czyli sprawdza też, czy jest wgle możliwość zarezerwowania hotelu
    } catch(const invalid_argument & e){
    QMessageBox::information(this, tr("Nieudana rezerwacja"),
                tr(e.what()));
} catch(const NoParent & e){
    QMessageBox::information(this, tr("Nieudana rezerwacja"),
                tr(e.what()));
}
}
