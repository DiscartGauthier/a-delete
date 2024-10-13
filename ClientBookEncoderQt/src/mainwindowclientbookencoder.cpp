#include "mainwindowclientbookencoder.h"
#include "ui_mainwindowclientbookencoder.h"
#include "unistd.h"
#include <QInputDialog>
#include <QMessageBox>
#include <iostream>
#include "TCP.h"
#include "OBEP.h"
using namespace std;

int sClient;
char IpServeur[50] = "0.0.0.0";

bool OBEP_Login_Client(const char* user, const char* password);
void OBEP_Logout();
void OBEP_Operation(char op);
void Echange(char* requete,char* reponse);

MainWindowClientBookEncoder::MainWindowClientBookEncoder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClientBookEncoder)
{
    ui->setupUi(this);
    ::close(2);

    //this->setFixedSize(1068, 301);

    // Configuration de la table des employes (Personnel Garage)
    ui->tableWidgetEncodedBooks->setColumnCount(9);
    ui->tableWidgetEncodedBooks->setRowCount(0);
    QStringList labelsTableEmployes;
    labelsTableEmployes << "Id" << "Titre" << "Auteur" << "Sujet" << "ISBN" << "Pages" << "Année" << "Prix" << "Stock";
    ui->tableWidgetEncodedBooks->setHorizontalHeaderLabels(labelsTableEmployes);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setVisible(true);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetEncodedBooks->verticalHeader()->setVisible(false);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setStyleSheet("background-color: lightyellow");
    int columnWidths[] = {35, 250, 200, 200, 150, 50, 50, 50, 40};
    for (int col = 0; col < 9; ++col)
        ui->tableWidgetEncodedBooks->setColumnWidth(col, columnWidths[col]);

    this->logoutOk();

    // Connexion sur le serveur
    if((sClient = ClientSocket(IpServeur,50000))==-1)
    {
        perror("Erreur de ClientSocket");
        exit(1);
    }
    printf("Client connecté sur le serveur.\n");

    
}

MainWindowClientBookEncoder::~MainWindowClientBookEncoder() {
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des livres encodés (ne pas modifier) ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::addTupleTableBooks(int id,
                                                     string title,
                                                     string author,
                                                     string subject,
                                                     string isbn,
                                                     int pageCount,
                                                     int publishYear,
                                                     float price,
                                                     int stockQuantity)
{
    int nb = ui->tableWidgetEncodedBooks->rowCount();
    nb++;
    ui->tableWidgetEncodedBooks->setRowCount(nb);
    ui->tableWidgetEncodedBooks->setRowHeight(nb-1,10);

    // id
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(id));
    ui->tableWidgetEncodedBooks->setItem(nb-1,0,item);

    // title
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setText(QString::fromStdString(title));
    ui->tableWidgetEncodedBooks->setItem(nb-1,1,item);

    // author
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(author));
    ui->tableWidgetEncodedBooks->setItem(nb-1,2,item);

    // subject
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(subject));
    ui->tableWidgetEncodedBooks->setItem(nb-1,3,item);

    // isbn
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(isbn));
    ui->tableWidgetEncodedBooks->setItem(nb-1,4,item);

    // pageCount
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(pageCount));
    ui->tableWidgetEncodedBooks->setItem(nb-1,5,item);

    // publishYear
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(publishYear));
    ui->tableWidgetEncodedBooks->setItem(nb-1,6,item);

    // price
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(price));
    ui->tableWidgetEncodedBooks->setItem(nb-1,7,item);

    // stockQuantity
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(stockQuantity));
    ui->tableWidgetEncodedBooks->setItem(nb-1,8,item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::clearTableBooks() {
    ui->tableWidgetEncodedBooks->setRowCount(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles des comboboxes (ne pas modifier) //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::addComboBoxAuthors(string author){
    ui->comboBoxAuthors->addItem(QString::fromStdString(author));
}

string MainWindowClientBookEncoder::getSelectionAuthor() const {
    return ui->comboBoxAuthors->currentText().toStdString();
}

void MainWindowClientBookEncoder::clearComboBoxAuthors() {
    ui->comboBoxAuthors->clear();
}

void MainWindowClientBookEncoder::addComboBoxSubjects(string subject){
    ui->comboBoxSubjects->addItem(QString::fromStdString(subject));
}

string MainWindowClientBookEncoder::getSelectionSubject() const {
    return ui->comboBoxSubjects->currentText().toStdString();
}

void MainWindowClientBookEncoder::clearComboBoxSubjects() {
    ui->comboBoxSubjects->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonction utiles de la fenêtre (ne pas modifier) ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientBookEncoder::getTitle() const {
    return ui->lineEditTitle->text().toStdString();
}

string MainWindowClientBookEncoder::getIsbn() const {
    return ui->lineEditIsbn->text().toStdString();
}

int MainWindowClientBookEncoder::getPageCount() const {
    return ui->spinBoxPageCount->value();
}

float MainWindowClientBookEncoder::getPrice() const {
    return ui->doubleSpinBoxPrice->value();
}

int MainWindowClientBookEncoder::getPublishYear() const {
    return ui->spinBoxPublishYear->value();
}

int MainWindowClientBookEncoder::getStockQuantity() const {
    return ui->spinBoxStockQuantity->value();
}

void MainWindowClientBookEncoder::loginOk() {
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonAddBook->setEnabled(true);
    ui->pushButtonAddAuthor->setEnabled(true);
    ui->pushButtonAddSubject->setEnabled(true);
    ui->actionLogin->setEnabled(false);
    ui->actionLogout->setEnabled(true);
}

void MainWindowClientBookEncoder::logoutOk() {
    ui->pushButtonClear->setEnabled(false);
    ui->pushButtonAddBook->setEnabled(false);
    ui->pushButtonAddAuthor->setEnabled(false);
    ui->pushButtonAddSubject->setEnabled(false);
    ui->actionLogin->setEnabled(true);
    ui->actionLogout->setEnabled(false);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier) ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::dialogMessage(const string& title,const string& message) {
   QMessageBox::information(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::dialogError(const string& title,const string& message) {
   QMessageBox::critical(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientBookEncoder::dialogInputText(const string& title,const string& question) {
    return QInputDialog::getText(this,QString::fromStdString(title),QString::fromStdString(question)).toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MainWindowClientBookEncoder::dialogInputInt(const string& title,const string& question) {
    return QInputDialog::getInt(this,QString::fromStdString(title),QString::fromStdString(question));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions gestion des boutons et items de menu (TO DO) /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::on_pushButtonAddAuthor_clicked() {
    string lastName = this->dialogInputText("Nouvel auteur","Nom ?");
    string firstName = this->dialogInputText("Nouvel auteur","Prénom ?");
    string birthDate = this->dialogInputText("Nouvel auteur","Date de naissance (yyyy-mm-dd) ?");
    cout << "Nom : " << lastName << endl;
    cout << "Prénom : " << firstName << endl;
    cout << "Date de naissance : " << birthDate << endl;

    char requete[200], reponse[200];
    sprintf(requete, "ADD_AUTHOR#%s#%s#%s", lastName.c_str(), firstName.c_str(), birthDate.c_str());
    Echange(requete, reponse);

    if (strstr(reponse, "ok") != NULL) {
        this->dialogMessage("Sucess", "Auteur ajouté avec succès !");
    } else {
        this->dialogError("Ajout", "Erreur lors de l'ajout de l'auteur.");
    }

    loadSubjects();
}

void MainWindowClientBookEncoder::on_pushButtonAddSubject_clicked() {
    string name = this->dialogInputText("Nouveau sujet","Nom ?");
    cout << "Nom : " << name << endl;

    char requete[200], reponse[200];
    sprintf(requete, "ADD_SUBJECT#%s", name.c_str());
    Echange(requete, reponse);

    if (strstr(reponse, "ok") != NULL) {
        this->dialogMessage("Sucess", "Sujet ajouté avec succès !");
    } else {
        this->dialogError("Ajout", "Erreur lors de l'ajout du sujet.");
    }

    loadSubjects();

}

void MainWindowClientBookEncoder::on_pushButtonAddBook_clicked() {
    cout << "title = " << this->getTitle() << endl;
    cout << "Isbn = " << this->getIsbn() << endl;
    cout << "PageCount = " << this->getPageCount() << endl;
    cout << "Price = " << this->getPrice() << endl;
    cout << "PublishYear = " << this->getPublishYear() << endl;
    cout << "Stock = " << this->getStockQuantity() << endl;

    cout << "selection auteur = " << this->getSelectionAuthor() << endl;
    cout << "selection sujet  = " << this->getSelectionSubject() << endl;

    //Construire une requete pour envoie
    char requete[500], reponse[200], fullNameCast[100];
    char *authorFirstName, *authorLastName;

    string authorFullName;
    authorFullName = this->getSelectionAuthor().c_str();
    strcpy(fullNameCast, authorFullName.c_str());



    authorFirstName = strtok(fullNameCast, " ");
    authorLastName = strtok(NULL, " ");

    sprintf(requete, "ADD_BOOK##%s#%s#%d#%f#%d#%d#%s#%s#%s",
            this->getTitle().c_str(),
            this->getIsbn().c_str(),
            this->getPageCount(),
            this->getPrice(),
            this->getPublishYear(),
            this->getStockQuantity(),
            authorFirstName,
            authorLastName,
            this->getSelectionSubject().c_str());

    Echange(requete, reponse);


    if (strstr(reponse, "ok") != NULL) {
        this->dialogMessage("Sucess", "Sujet ajouté avec succès !");
    } else {
        this->dialogError("Ajout", "Erreur lors de l'ajout du sujet.");
    }

    loadBooks();

}

void MainWindowClientBookEncoder::on_pushButtonClear_clicked() {
    ui->lineEditTitle->clear();
    ui->lineEditIsbn->clear();
    ui->spinBoxPageCount->setValue(0);
    ui->doubleSpinBoxPrice->setValue(0);
    ui->spinBoxPublishYear->setValue(0);
    ui->spinBoxStockQuantity->setValue(0);
}

void MainWindowClientBookEncoder::on_actionLogin_triggered() {
    
    // vérification qu'un LOGIN est bien encodé
    string login = this->dialogInputText("Entrée en session","Login ?"); //le caractère ' ' fait bugger
    if(strlen(login.c_str()) == 0)
    {
        this->dialogError("LOGIN","Veuillez entrer un identifiant !");
    }
    else
    {
        // vérification qu'un mdp est bien encodé
        string password = this->dialogInputText("Entrée en session","Password ?");

        if(strlen(password.c_str()) == 0)
        {
            this->dialogError("LOGIN","Veuillez entrer un mot de passe !");
        }
        else
        {
            //envoyer au serveur la commande + vérif
            if(OBEP_Login_Client(login.c_str(),password.c_str())==false)
            {
                this->dialogError("LOGIN","Mauvais identifiants !");
            }
            else
            {
                this->loginOk();

                loadAuthors();
                loadSubjects();
                loadBooks();
            }

        }

    }
    
}

void MainWindowClientBookEncoder::on_actionLogout_triggered() {
    // envoyer au serveur la commande
    OBEP_Logout();
    this->logoutOk();
    
    
}

void MainWindowClientBookEncoder::on_actionQuitter_triggered(){
    QApplication::exit(0);
}




// gestion du protocole OBEP

bool OBEP_Login_Client(const char* user,const char* password)
{
    char requete[200],reponse[200];
    bool onContinue = true;

    // Construction de la requête 
    sprintf(requete,"LOGIN#%s#%s",user,password);

    // Echange entre le serveur et client
    Echange(requete,reponse);

    char *ptr = strtok(reponse,"#");
    ptr = strtok(NULL,"#");
    if(strcmp(ptr,"ok") == 0) printf("Login OK. \n");
    else
    {
        ptr = strtok(NULL,"#");
        printf("Erreur de login: %s\n",ptr);
        onContinue=false;
    }

    return onContinue;
}

void OBEP_Logout()
{
    char requete[200],reponse[200];

    // ***** Construction de la requete *********************
    sprintf(requete,"LOGOUT#");
    // ***** Envoi requete + réception réponse **************
    Echange(requete,reponse);
}


void OBEP_Operation(char op)
{

}

// Définition de échange //
void Echange(char* requete,char* reponse)
{
    int nbEcrits=0, nbLus=0;
    

    // ***** Envoi de la requete ****************************
    if ((nbEcrits = Send(sClient,requete,strlen(requete))) == -1)
    {
        perror("Erreur de Send");
        close(sClient);
        exit(1);
    }
    // ***** Attente de la reponse **************************
    if ((nbLus = Receive(sClient,reponse)) < 0)
    {
        perror("Erreur de Receive");
        close(sClient);
        exit(1);
    }
    if (nbLus == 0)
    {
        printf("Serveur arrete, pas de reponse reçue...\n");
        close(sClient);
        exit(1);
    }
    reponse[nbLus] = 0;
}

void MainWindowClientBookEncoder::loadBooks()
{
    char requete[200], reponse[200];
    sprintf(requete, "GET_BOOKS");

    // Echange entre le serveur et client
    Echange(requete, reponse);

    char *ptr = strtok(reponse, "#");
    ptr = strtok(NULL, "#");
    if (strcmp(ptr, "ok") == 0) {

        clearTableBooks();
        int id, pageCount, publishYear, stock;
        string title, author, subject, isbn;
        float price;


        while ((ptr = strtok(NULL, "#")) != NULL) {

            id = atoi(ptr);
            ptr = strtok(NULL, "#");

            title = ptr;
            ptr = strtok(NULL, "#");

            author = ptr;
            ptr = strtok(NULL, "#");

            subject = ptr;
            ptr = strtok(NULL, "#");

            isbn = ptr;
            ptr = strtok(NULL, "#");

            pageCount = atoi(ptr);
            ptr = strtok(NULL, "#");

            publishYear = atoi(ptr);
            ptr = strtok(NULL, "#");

            price = atof(ptr);
            ptr = strtok(NULL, "#");

            stock = atoi(ptr);

            // Ajouter à la table
            addTupleTableBooks(id, title, author, subject, isbn, pageCount, publishYear, price, stock);
        }
    }
}

void MainWindowClientBookEncoder::loadAuthors()
{
    char requete[200], reponse[200];
    sprintf(requete, "GET_AUTHORS");

    // Echange entre le serveur et client
    Echange(requete, reponse);

    char *ptr = strtok(reponse, "#");
    ptr = strtok(NULL, "#");
    if (strcmp(ptr, "ok") == 0) {

        clearTableBooks();
        int id;
        string author, firstName, lastName;
        char *firstNameCast, *lastNameCast;


        while ((ptr = strtok(NULL, "#")) != NULL) {

            id = atoi(ptr);
            ptr = strtok(NULL, "#");

            firstName = ptr;
            ptr = strtok(NULL, "#");

            lastName = ptr;



            strcpy(firstNameCast, firstName.c_str());
            strcpy(lastNameCast, lastName.c_str());


            strcat(firstNameCast, " ");
            strcat(firstNameCast, lastNameCast);


            // Ajouter à la combobox
            addComboBoxAuthors(author);
        }
    }
}

void MainWindowClientBookEncoder::loadSubjects()
{
        char requete[200], reponse[200];
    sprintf(requete, "GET_SUBJECTS");

    // Echange entre le serveur et client
    Echange(requete, reponse);

    char *ptr = strtok(reponse, "#");
    ptr = strtok(NULL, "#");
    if (strcmp(ptr, "ok") == 0) {

        clearTableBooks();
        int id;
        string name;


        while ((ptr = strtok(NULL, "#")) != NULL) {

            id = atoi(ptr);
            ptr = strtok(NULL, "#");

            name = ptr;


            // Ajouter à la combobox
            addComboBoxSubjects(name);
        }
    }
}
