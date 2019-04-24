#include "../headers/resultsview.h"
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QDebug>
#include <QFormLayout>
#include <vector>
#include <sstream>
#include <iostream>

ResultsView::ResultsView(DataBase *m_db,std::string m_surNameName,QWidget* parent) : QDialog(parent){
    db=m_db;
    surNameName=m_surNameName;
    setWindowTitle("Zarezerwowane hotele");
    initView();
}

void ResultsView::initView(){
    QHBoxLayout* hbox = new QHBoxLayout;
    std::vector<QFormLayout*> layouts;
    layouts.push_back(new QFormLayout);
    layouts[layouts.size()-1]->addRow(new QLabel("Miasto"));
    layouts[layouts.size()-1]->addRow(new QLabel("Hotel"));
    layouts[layouts.size()-1]->addRow(new QLabel("Pierwszy dzień"));
    layouts[layouts.size()-1]->addRow(new QLabel("Ostatni dzień"));
    layouts[layouts.size()-1]->addRow(new QLabel("Imię"));
    layouts[layouts.size()-1]->addRow(new QLabel("Nazwisko"));
    layouts[layouts.size()-1]->addRow(new QLabel("Nr pokoju"));


    hbox->addLayout(layouts[layouts.size()-1]);
    std::string line;

    vector<string> yourReservations=db->getReservations(surNameName);
    for (auto it=yourReservations.begin();it!=yourReservations.end();++it){
        line=*it;
        layouts.push_back(new QFormLayout);
        std::stringstream iss(line);
        std::vector<std::string> results;
        std::string segment;
        while(std::getline(iss, segment, '~'))
        {
            results.push_back(segment);
        }
        for (auto it=results.begin();it!=results.end();++it)
            layouts[layouts.size()-1]->addRow(new QLabel(QString::fromStdString(*it)));
        hbox->addLayout(layouts[layouts.size()-1]);
    }

    Q_ASSERT(hbox->parent() == 0);
    setLayout(hbox);                        /* Zmienia rodzica już wcześniej rozmieszczonych widżetów. */
    Q_ASSERT(hbox->parent() == this);
}
