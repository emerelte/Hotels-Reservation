#ifndef RESULTSVIEW_H
#define RESULTSVIEW_H

#include "hotels.h"
#include <QDialog>
#include <QObject>
#include <QLabel>
#include <fstream>
#include <string>
#include <vector>

class ResultsView : public QDialog {
        Q_OBJECT
    public:
        explicit ResultsView(DataBase *m_db,std::string surNameName,QWidget* parent = 0);
        ~ResultsView(){
            inFile.close();
            for (auto it=reservations.begin();it!=reservations.end();++it)
                delete *it;
        }
        void initView();
    protected slots:
     //   void accept();
    private:
        std::string inFileName;
        std::ifstream inFile;

        DataBase * db;
        string surNameName;

        std::vector<QLabel*> reservations;
};

#endif // RESULTSVIEW_H
