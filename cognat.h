#ifndef COGNAT_H
#define COGNAT_H

#include <QCloseEvent>
#include <QColor>
#include <QMainWindow>
#include <QPoint>
#include <QString>

class QAction;
class QActionGroup;
class QComboBox;
class QLabel;
class QMenu;
class QToolBar;
class Database;
class MainWidget;

class COGNAT : public QMainWindow
{
    Q_OBJECT

public:
    COGNAT();

    void createContextMenu(const QPoint &point);
    void createColorDialog(const QString &architecture, const QColor &initial);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void load(); // 1.1
    void save(); // 1.2 ###
    void saveAs(); // 1.3
    void loadRecentProject(); // 1.4
    void searchGenes(); // 2.1 ###
    void specifyDatabase(); // 2.2
    void sortGenomesByTaxonomy(bool checked); // 3.1 ###
    void markGenesOfMembraneProteins(bool checked); // 3.2 ###
    void examineOrthologsAndParalogs(bool checked); // 3.3
    void showIntergenes(bool checked); // 3.4 ###
    void colorByDomainArchitectures(); // 3.5.1
    void colorByCOGArchitectures(); // 3.5.2
    void colorByDomains(); // 3.5.3
    void colorByCOGs(); // 3.5.4
    void loadColorScheme(); // 3.6
    void colorArchitectures(); // 3.7
    //void setPdfResolution();
    void setNeighborhoodRadius(); // 3.8
    void setValues(); // 3.9
    void reconstructTree(); // 4.1
    void setPhylumColor(); // 4.2
    void about(); // 5.1
    void setColorGenesComboBox(QAction *action); // 3.5
    void setColorGenesGroup(const int &index); // 3.5
    void modifyNeighborhoodRadius(const int &radius); // 3.8
    void color();
    void decolor();
    /*void newFile();
    void open();
    bool save();
    bool saveAs();
    void openRecentFile();
    void updateStatusBar();
    void spreadsheetModified();*/

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void setGeometry();
    void readSettings();
    void writeSettings();
    void writeColorToSettings(const QString &architecture, const QColor &color);
    bool okToContinue();
    void loadTextFile(const QString &fileName);
    void loadSearchedGenes(const QString &fileName);
    void loadProject(const QString &fileName);
    void savePdf(const QString &fileName);
    void saveProject(const QString &fileName);
    /*bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();*/

    Database *database;

    enum { MaxRecentProjects = 5 };
    enum { MaxNeighborhoodRadius = 10}; // Should be greater than 0.
    enum InputType { Default, Search, Phylogeny };

    InputType inputType;

    int pdfWidth;
    int pdfHeight;
    int neighborhoodRadius;
    int minimumBitScore;
    int minimumRelativeLength;
    int maximumOverlapping;
    int trustworthyBitScore;

    MainWidget *mainWidget;

    QAction *loadAction; // 1.1
    QAction *saveAction; // 1.2 ###
    QAction *saveAsAction; // 1.3
    QAction *separatorAction;
    QAction *loadRecentProjectActions[MaxRecentProjects]; // 1.4
    QAction *exitAction; // 1.5
    QAction *searchGenesAction; // 2.1 ###
    QAction *specifyDatabaseAction; // 2.2
    QAction *sortGenomesByTaxonomyAction; // 3.1 ###
    QAction *markGenesOfMembraneProteinsAction; // 3.2 ###
    QAction *examineOrthologsAndParalogsAction; // 3.3
    QAction *showIntergenesAction; // 3.4 ###
    QAction *colorByDomainArchitecturesAction; // 3.5.1
    QAction *colorByCOGArchitecturesAction; // 3.5.2
    QAction *colorByDomainsAction; // 3.5.3
    QAction *colorByCOGsAction; // 3.5.4
    QAction *loadColorSchemeAction; // 3.6
    QAction *colorArchitecturesAction; // 3.7
    //QAction *setPdfResolutionAction;
    QAction *setNeighborhoodRadiusAction; // 3.8
    QAction *setValuesAction; // 3.9
    QAction *reconstructTreeAction; // 4.1
    QAction *setPhylumColorAction; // 4.2
    QAction *aboutAction; // 5.1
    QAction *colorAction;
    QAction *decolorAction;

    QActionGroup *colorGenesGroup; // 3.5

    QComboBox *colorGenesComboBox; // 3.5
    QComboBox *neighborhoodRadiusComboBox; // 3.8

    QLabel *colorGenesLabel; // 3.5
    QLabel *neighborhoodRadiusLabel; // 3.8

    QMenu *fileMenu; // 1
    QMenu *databaseMenu; // 2
    QMenu *optionsMenu; // 3
    QMenu *colorGenesSubMenu; // 3.5
    QMenu *phylogenyMenu; // 4
    QMenu *helpMenu; // 5

    QString databasePath;
    QString proteinDomain;
    QString loadPath;
    QString savePath;

    QToolBar *fileToolBar; // 1
    QToolBar *databaseToolBar; // 2
    QToolBar *optionsToolBar; // 3
};

#endif
