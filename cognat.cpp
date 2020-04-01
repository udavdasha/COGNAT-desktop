#include <QActionGroup>
#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPrinter>
#include <QSettings>
#include <QSizeF>
#include <QStatusBar>
#include <QStringList>
#include <QToolBar>
#include "cognat.h"
#include "colorarchitecturesdialog.h"
#include "database.h"
#include "loadcolorschemedialog.h"
#include "mainwidget.h"
#include "searchgenesdialog.h"
#include "setradiusdialog.h"
//#include "setresolutiondialog.h"
#include "setvaluesdialog.h"
#include "specifydatabasedialog.h"

COGNAT::COGNAT()
{
    readSettings();
    createActions();
    createMenus();
    createToolBars();
    statusBar();
    setGeometry();
    setFocus();
    setFocusPolicy(Qt::StrongFocus);

    database = new Database(databasePath, MaxNeighborhoodRadius, this);
    database->setObjectName(QString(DATABASE_NAME));
    database->setWindowTitle("COGNAT Database informs...");
    mainWidget = NULL;

    setWindowIcon(QIcon(":/COGNAT_icon.ico"));
    setWindowTitle(QString("COGNAT (COmparative Gene Neighborhoods Analysis Tool)"));
    //setCurrentFile("");
}

void COGNAT::createContextMenu(const QPoint &point)
{
    QMenu contextMenu(this);
    contextMenu.addAction(colorAction);
    contextMenu.addAction(decolorAction);
    contextMenu.exec(point);
}

void COGNAT::createColorDialog(const QString &architecture, const QColor &initial)
{
    QColor color = QColorDialog::getColor(initial, this, tr("Set Color"), QColorDialog::DontUseNativeDialog);
    if (color.isValid()) {
        writeColorToSettings(architecture, color);
        setColorGenesGroup(colorGenesComboBox->currentIndex());
    }
}

void COGNAT::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

/*void COGNAT::newFile()
{
    if (okToContinue()) {
        spreadsheet->clear();
        setCurrentFile("");
    }
}

void COGNAT::open()
{
    if (okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open Spreadsheet"), ".",
                                   tr("Spreadsheet files (*.sp)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool COGNAT::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void COGNAT::openRecentFile()
{
    if (okToContinue()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            loadFile(action->data().toString());
    }
}

void COGNAT::updateStatusBar()
{
    locationLabel->setText(spreadsheet->currentLocation());
    formulaLabel->setText(spreadsheet->currentFormula());
}

void COGNAT::spreadsheetModified()
{
    setWindowModified(true);
    updateStatusBar();
}*/

void COGNAT::load() // 1.1
{
    if (okToContinue()) {
        if (database->isValid()) {
            if (loadPath.isEmpty()) {
                loadPath = QDir::homePath();
            }
            QString fileName = QFileDialog::getOpenFileName(this, tr("Load"), loadPath, tr("All files (*.*);;Text files (*.txt)"));
            fileName = QFileInfo(fileName).canonicalFilePath();
            if (!fileName.isEmpty()) {
                loadPath = QFileInfo(fileName).canonicalPath();
                QString extension = QFileInfo(fileName).suffix().toLower();
                if (extension == "ols") {
                    loadProject(fileName);
                } else {
                    loadTextFile(fileName);
                }
            }
        } else {
            /*QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not load a text file or a project.\n\n"
                                    "Please, specify a path to a database\n"
                                    "using the Database menu."));*/
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not load a text file.\n\n"
                                    "Please, specify a path to a database\n"
                                    "using the Database menu."));
        }
    }
}

void COGNAT::save() // 1.2 ###
{
    // TEMPORARY CODE !!!
    if (database->textFileWasLoaded()) {
        if (savePath.isEmpty()) {
            savePath = QDir::homePath();
        }
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), savePath, tr("PDF (*.pdf);;All files (*.*)"));
        if (!fileName.isEmpty()) {
            QString extension = QFileInfo(fileName).suffix().toLower();
            if (extension == "pdf") {
                savePdf(fileName);
            } else if (extension == "ols") {
                saveProject(fileName);
            }
        }
    }
    // TEMPORARY CODE !!!
}

void COGNAT::saveAs() // 1.3
{
    /*if (database->textFileWasLoaded()) {
        if (savePath.isEmpty()) {
            savePath = QDir::homePath();
        }
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), savePath, tr("PDF (*.pdf);;Text files (*.txt)"));
        if (!fileName.isEmpty()) {
            QString extension = QFileInfo(fileName).suffix().toLower();
            if (extension == "pdf") {
                savePdf(fileName);
            } else if (extension == "txt") {
                //saveTextFile(fileName);
            }
        }
    }*/
}

void COGNAT::loadRecentProject() {} // 1.4

void COGNAT::searchGenes() // 2.1 ###
{
    if (okToContinue()) {
        if (database->isValid()) {
            SearchGenesDialog dialog(this);
            if (dialog.exec()) {
                proteinDomain = dialog.specifiedDomain();
                if (database->textFileWasWritten(proteinDomain, minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, "./tmp.txt")) {
                    loadSearchedGenes("./tmp.txt");
                    QFile::remove("./tmp.txt");
                } else {
                    QMessageBox::warning(this, tr("Warning"),
                                         tr("%1.")
                                         .arg(database->errorString()));
                }
            }
        } else {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not search genes.\n\n"
                                    "Please, specify a path to a database\n"
                                    "using the Database menu."));
        }
    }
}

void COGNAT::specifyDatabase() // 2.2
{
    if (!database->exists()) {
        database->setPath(QString());
    }
    SpecifyDatabaseDialog dialog(database->canonicalPath(), this);
    if (dialog.exec()) {
        databasePath = dialog.specifiedPath();
        database->setPath(databasePath);
        statusBar()->showMessage(tr("A database '%1' was specified")
                                 .arg(database->canonicalPath()));
    }
}

void COGNAT::sortGenomesByTaxonomy(bool checked) // 3.1 ###
{
    if (database->textFileWasLoaded()) {
        if (checked) {
            mainWidget->setTreeTopWidget();
            statusBar()->showMessage(tr("Genomes were sorted by taxonomy"));
        } else {
            switch (inputType) {
            case Default:
                mainWidget->setTableTopWidget();
                statusBar()->showMessage(tr("Genomes were sorted by default"));
                break;
            case Search:
                mainWidget->setTableTopWidget();
                statusBar()->showMessage(tr("Genomes were sorted by a bit score"));
                break;
            case Phylogeny:
                mainWidget->setTextTopWidget();
                statusBar()->showMessage(tr("Genomes were sorted by a phylogenetic tree"));
                break;
            default:
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Something bad happened.\n\n"
                                        "Please, reload a text file or a project."));
                break;
            }
        }
        mainWidget->setNeighborhoodRadius();
        showIntergenes(showIntergenesAction->isChecked());
        setColorGenesGroup(colorGenesComboBox->currentIndex());
        markGenesOfMembraneProteins(markGenesOfMembraneProteinsAction->isChecked());
    }
}

void COGNAT::markGenesOfMembraneProteins(bool checked) // 3.2 ###
{
    if (database->textFileWasLoaded()) {
        if (checked) {
            if (!database->annotationWasLoaded(Database::Membrane)) {
                if (!database->loadAnnotation(Database::Membrane)) {
                    QMessageBox::warning(this, tr("Warning"),
                                         tr("Could not load a membrane annotation.\n\n%1.")
                                         .arg(database->errorString()));
                } else {
                    mainWidget->markGenesOfMembraneProteins(true);
                    statusBar()->showMessage(tr("Genes of presumably membrane proteins were marked with hatching"));
                }
            } else {
                mainWidget->markGenesOfMembraneProteins(true);
                statusBar()->showMessage(tr("Genes of presumably membrane proteins were marked with hatching"));
            }
        } else if (database->annotationWasLoaded(Database::Membrane)) {
            mainWidget->markGenesOfMembraneProteins(false);
            statusBar()->showMessage(tr("Genes of presumably membrane proteins are not marked"));
        }
        setFocus();
    }
}

void COGNAT::examineOrthologsAndParalogs(bool checked) // 3.3
{
    if (database->textFileWasLoaded()) { // ??? Necessary?
        if (checked) {
            statusBar()->showMessage(tr("An examination of orthologs and paralogs was turned ON"));
        } else {
            statusBar()->showMessage(tr("An examination of orthologs and paralogs was turned OFF"));
        }
    }
}

void COGNAT::showIntergenes(bool checked) // 3.4 ###
{
    if (database->textFileWasLoaded()) {
        mainWidget->showIntergenes(checked);
        if (checked) {
            statusBar()->showMessage(tr("Intergenic spaces were shown as lines"));
        } else {
            statusBar()->showMessage(tr("Intergenic spaces are not shown"));
        }
        setFocus();
    }
}

void COGNAT::colorByDomainArchitectures() // 3.5.1
{
    if (database->textFileWasLoaded()) {
        if (!database->annotationWasLoaded(Database::Domains)) {
            if (!database->loadAnnotation(Database::Domains)) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Could not load a domain annotation.\n\n%1.")
                                     .arg(database->errorString()));
            } else {
                database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::Domains);
                mainWidget->deleteAnnotation();
                mainWidget->colorByDomainArchitectures();
                statusBar()->showMessage(tr("Genes were colored by protein domain architectures"));
            }
        } else {
            database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::Domains);
            mainWidget->deleteAnnotation();
            mainWidget->colorByDomainArchitectures();
            statusBar()->showMessage(tr("Genes were colored by protein domain architectures"));
        }
        setFocus();
    }
}

void COGNAT::colorByCOGArchitectures() // 3.5.2
{
    if (database->textFileWasLoaded()) {
        if (!database->annotationWasLoaded(Database::COGs)) {
            if (!database->loadAnnotation(Database::COGs)) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Could not load a COG annotation.\n\n%1.")
                                     .arg(database->errorString()));
            } else {
                database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::COGs);
                mainWidget->deleteAnnotation();
                mainWidget->colorByCOGArchitectures();
                statusBar()->showMessage(tr("Genes were colored by protein COG architectures"));
            }
        } else {
            database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::COGs);
            mainWidget->deleteAnnotation();
            mainWidget->colorByCOGArchitectures();
            statusBar()->showMessage(tr("Genes were colored by protein COG architectures"));
        }
        setFocus();
    }
}

void COGNAT::colorByDomains() // 3.5.3
{
    if (database->textFileWasLoaded()) {
        if (!database->annotationWasLoaded(Database::Domains)) {
            if (!database->loadAnnotation(Database::Domains)) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Could not load a domain annotation.\n\n%1.")
                                     .arg(database->errorString()));
            } else {
                database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::Domains);
                mainWidget->deleteAnnotation();
                mainWidget->colorByDomains();
                markGenesOfMembraneProteins(markGenesOfMembraneProteinsAction->isChecked());
                statusBar()->showMessage(tr("Genes were colored by protein domains"));
            }
        } else {
            database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::Domains);
            mainWidget->deleteAnnotation();
            mainWidget->colorByDomains();
            markGenesOfMembraneProteins(markGenesOfMembraneProteinsAction->isChecked());
            statusBar()->showMessage(tr("Genes were colored by protein domains"));
        }
        setFocus();
    }
}

void COGNAT::colorByCOGs() // 3.5.4
{
    if (database->textFileWasLoaded()) {
        if (!database->annotationWasLoaded(Database::COGs)) {
            if (!database->loadAnnotation(Database::COGs)) {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Could not load a COG annotation.\n\n%1.")
                                     .arg(database->errorString()));
            } else {
                database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::COGs);
                mainWidget->deleteAnnotation();
                mainWidget->colorByCOGs();
                markGenesOfMembraneProteins(markGenesOfMembraneProteinsAction->isChecked());
                statusBar()->showMessage(tr("Genes were colored by protein COGs"));
            }
        } else {
            database->updateAnchors(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, Database::COGs);
            mainWidget->deleteAnnotation();
            mainWidget->colorByCOGs();
            markGenesOfMembraneProteins(markGenesOfMembraneProteinsAction->isChecked());
            statusBar()->showMessage(tr("Genes were colored by protein COGs"));
        }
        setFocus();
    }
}

void COGNAT::loadColorScheme() // 3.6
{
    LoadColorSchemeDialog dialog(this);
    if (dialog.exec()) {
        bool loaded = false;
        QString colorScheme = dialog.loadedScheme();
        QStringList list = colorScheme.split("\n");
        foreach (const QString &line, list) {
            QString architecture = line.section("\t", 0, 0).trimmed();
            QString name = line.section("\t", 1, 1).trimmed();
            QColor color(name);
            if (!architecture.isEmpty() && color.isValid()) {
                writeColorToSettings(architecture, color);
                loaded = true;
            } else {
                architecture = line.section("\t", 1, 1).trimmed();
                name = line.section("\t", 0, 0).trimmed();
                color = QColor(name);
                if (!architecture.isEmpty() && color.isValid()) {
                    writeColorToSettings(architecture, color);
                    loaded = true;
                }
            }
        }
        if (database->textFileWasLoaded()) {
            setColorGenesGroup(colorGenesComboBox->currentIndex());
        }
        if (loaded) {
            statusBar()->showMessage(tr("A color scheme was loaded"));
        } else {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not load a color scheme."));
        }
    }
}

void COGNAT::colorArchitectures() // 3.7
{
    ColorArchitecturesDialog dialog(this);
    if (dialog.exec()) {
        QString proteinArchitectures = dialog.loadedArchitectures();
        if (database->textFileWasLoaded()) {
            database->loadArchitectures(proteinArchitectures);
            setColorGenesGroup(colorGenesComboBox->currentIndex());
            statusBar()->showMessage(tr("Loaded protein architectures were colored"));
        } else {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not load protein architectures.\n\n"
                                    "There is nothing to color."));
        }
    }
}

/*void COGNAT::setPdfResolution() //
{
    SetResolutionDialog dialog(pdfWidth, pdfHeight, this);
    if (dialog.exec()) {
        pdfWidth = dialog.tunedWidth();
        pdfHeight = dialog.tunedHeight();
        statusBar()->showMessage(tr("Tuned resolution was saved"));
    }
}*/

void COGNAT::setNeighborhoodRadius() // 3.8
{
    setFocus();
    SetRadiusDialog dialog(neighborhoodRadius, MaxNeighborhoodRadius, this);
    if (dialog.exec()) {
        neighborhoodRadius = dialog.tunedRadius();
        neighborhoodRadiusComboBox->setCurrentIndex(neighborhoodRadius - 1);
        if (database->textFileWasLoaded()) {
            mainWidget->setRadius(neighborhoodRadius);
            mainWidget->setNeighborhoodRadius();
        }
        statusBar()->showMessage(tr("A number of genes in a neighborhood was tuned to %1 genes")
                                 .arg(neighborhoodRadius * 2 + 1));
    }
}

void COGNAT::setValues() // 3.9
{
    SetValuesDialog dialog(minimumBitScore, minimumRelativeLength, maximumOverlapping, trustworthyBitScore, this);
    if (dialog.exec()) {
        minimumBitScore = dialog.tunedMinimumScore();
        minimumRelativeLength = dialog.tunedMinimumLength();
        maximumOverlapping = dialog.tunedMaximumOverlapping();
        trustworthyBitScore = dialog.tunedTrustworthyScore();
        if (database->textFileWasLoaded()) {
            mainWidget->setMinimumScore(minimumBitScore);
            mainWidget->setMinimumLength(minimumRelativeLength);
            mainWidget->setMaximumOverlapping(maximumOverlapping);
            mainWidget->setTrustworthyScore(trustworthyBitScore);
            mainWidget->deleteAnnotation();
            setColorGenesGroup(colorGenesComboBox->currentIndex());
        }
        statusBar()->showMessage(tr("Tuned values were saved"));
    }
}

void COGNAT::reconstructTree() {} // 4.1

void COGNAT::setPhylumColor() {} // 4.2

void COGNAT::about() // 5.1
{
    QMessageBox::about(this, tr("About COGNAT"),
                       tr("<h2>COGNAT</h2>"
                          "<h3>COmparative Gene Neighborhoods Analysis Tool</h3>"
                          "<p>Copyright &copy; 2020, Molecular Bioenergetics Group</p>"
                          "<p>A.N.Belozersky Institute of Physico - Chemical Biology, Lomonosov MSU"));
}

void COGNAT::setColorGenesComboBox(QAction *action) // 3.5
{
    colorGenesComboBox->setCurrentIndex(colorGenesComboBox->findText(action->text()));
}

void COGNAT::setColorGenesGroup(const int &index) // 3.5
{
    colorGenesGroup->actions()[index]->setChecked(true);
    colorGenesGroup->actions()[index]->trigger();
}

void COGNAT::modifyNeighborhoodRadius(const int &index) // 3.8
{
    neighborhoodRadius = index + 1;
    if (database->textFileWasLoaded()) {
        mainWidget->setRadius(neighborhoodRadius);
        mainWidget->setNeighborhoodRadius();
        showIntergenes(showIntergenesAction->isChecked());
        setColorGenesGroup(colorGenesComboBox->currentIndex());
    }
    statusBar()->showMessage(tr("A number of genes in a neighborhood was tuned to %1 genes")
                             .arg((index + 1) * 2 + 1));
    setFocus();
}

void COGNAT::color()
{
    QString gi = mainWidget->currentGi();
    QString architecture = mainWidget->currentArchitecture();
    database->insertAnchor(gi, architecture);
    if (architecture.isEmpty()) {
        if (mainWidget->mode == MainWidget::Domains || mainWidget->mode == MainWidget::DomainArchitectures) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not color a selected gene.\n\n"
                                    "No significant Pfam-A matches."));
        } else if (mainWidget->mode == MainWidget::COGs || mainWidget->mode == MainWidget::COGArchitectures) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Could not color a selected gene.\n\n"
                                    "No significant COGs matches."));
        }
    }
    setColorGenesGroup(colorGenesComboBox->currentIndex());
}

void COGNAT::decolor()
{
    QString architecture = mainWidget->currentArchitecture();
    database->removeAnchor(architecture);
    setColorGenesGroup(colorGenesComboBox->currentIndex());
}

void COGNAT::createActions()
{
    // 1.1
    loadAction = new QAction(tr("Load..."), this);
    //loadAction->setStatusTip(tr("Load a text file of protein GIs or IDs written under each other or load a project"));
    loadAction->setStatusTip(tr("Load a text file of protein GIs or IDs written under each other"));
    connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));

    // 1.2 ###
    saveAction = new QAction(tr("Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    //saveAction->setStatusTip(tr("Save the project to a disk"));
    //saveAction->setToolTip(tr("Save the project to a disk"));
    saveAction->setStatusTip(tr("Save PDF"));
    saveAction->setToolTip(tr("Save PDF"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    // 1.3
    saveAsAction = new QAction(tr("Save As..."), this);
    saveAsAction->setStatusTip(tr("Save under a new name or format"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    // 1.4
    for (int i = 0; i < MaxRecentProjects; ++i)
    {
        loadRecentProjectActions[i] = new QAction(this);
        loadRecentProjectActions[i]->setVisible(false);
        connect(loadRecentProjectActions[i], SIGNAL(triggered()), this, SLOT(loadRecentProject()));
    }

    // 1.5
    exitAction = new QAction(tr("Exit"), this);
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    // 2.1 ###
    searchGenesAction = new QAction(tr("Search Genes..."), this);
    searchGenesAction->setIcon(QIcon(":/images/search.png"));
    searchGenesAction->setStatusTip(tr("Search genes of proteins containing a given domain or COG"));
    searchGenesAction->setToolTip(tr("Search genes of proteins containing a given domain or COG"));
    connect(searchGenesAction, SIGNAL(triggered()), this, SLOT(searchGenes()));

    // 2.2
    specifyDatabaseAction = new QAction(tr("Specify Database..."), this);
    specifyDatabaseAction->setStatusTip(tr("Specify a path to a database"));
    connect(specifyDatabaseAction, SIGNAL(triggered()), this, SLOT(specifyDatabase()));

    // 3.1 ###
    sortGenomesByTaxonomyAction = new QAction(tr("Sort Genomes By Taxonomy"), this);
    sortGenomesByTaxonomyAction->setCheckable(true);
    sortGenomesByTaxonomyAction->setChecked(false);
    sortGenomesByTaxonomyAction->setIcon(QIcon(":/images/sort.png"));
    sortGenomesByTaxonomyAction->setStatusTip(tr("Sort genomes by taxonomy"));
    sortGenomesByTaxonomyAction->setToolTip(tr("Sort genomes by taxonomy"));
    connect(sortGenomesByTaxonomyAction, SIGNAL(toggled(bool)), this, SLOT(sortGenomesByTaxonomy(bool)));

    // 3.2 ###
    markGenesOfMembraneProteinsAction = new QAction(tr("Mark Genes Of Membrane Proteins"), this);
    markGenesOfMembraneProteinsAction->setCheckable(true);
    markGenesOfMembraneProteinsAction->setChecked(false);
    markGenesOfMembraneProteinsAction->setIcon(QIcon(":/images/mark.png"));
    markGenesOfMembraneProteinsAction->setStatusTip(tr("Mark genes of presumably membrane proteins with hatching"));
    markGenesOfMembraneProteinsAction->setToolTip(tr("Mark genes of presumably membrane proteins with hatching"));
    connect(markGenesOfMembraneProteinsAction, SIGNAL(toggled(bool)), this, SLOT(markGenesOfMembraneProteins(bool)));

    // 3.3
    examineOrthologsAndParalogsAction = new QAction(tr("Examine Orthologs And Paralogs"), this);
    examineOrthologsAndParalogsAction->setCheckable(true);
    examineOrthologsAndParalogsAction->setChecked(true);
    examineOrthologsAndParalogsAction->setStatusTip(tr("Color/decolor all genes, which products have the same architecture"));
    connect(examineOrthologsAndParalogsAction, SIGNAL(toggled(bool)), this, SLOT(examineOrthologsAndParalogs(bool)));

    // 3.4 ###
    showIntergenesAction = new QAction(tr("Show Intergenic Spaces"), this);
    showIntergenesAction->setCheckable(true);
    showIntergenesAction->setChecked(true);
    showIntergenesAction->setIcon(QIcon(":/images/show.png"));
    showIntergenesAction->setStatusTip(tr("Show intergenic spaces"));
    showIntergenesAction->setToolTip(tr("Show intergenic spaces"));
    connect(showIntergenesAction, SIGNAL(toggled(bool)), this, SLOT(showIntergenes(bool)));

    // 3.5.1
    colorByDomainArchitecturesAction = new QAction(tr("By Domain Architectures"), this);
    colorByDomainArchitecturesAction->setCheckable(true);
    colorByDomainArchitecturesAction->setStatusTip(tr("Each domain architecture is assigned to a specific color"));
    connect(colorByDomainArchitecturesAction, SIGNAL(triggered()), this, SLOT(colorByDomainArchitectures()));

    // 3.5.2
    colorByCOGArchitecturesAction = new QAction(tr("By COG Architectures"), this);
    colorByCOGArchitecturesAction->setCheckable(true);
    colorByCOGArchitecturesAction->setStatusTip(tr("Each COG architecture is assigned to a specific color"));
    connect(colorByCOGArchitecturesAction, SIGNAL(triggered()), this, SLOT(colorByCOGArchitectures()));

    // 3.5.3
    colorByDomainsAction = new QAction(tr("By Domains"), this);
    colorByDomainsAction->setCheckable(true);
    colorByDomainsAction->setStatusTip(tr("Each domain is assigned to a specific color"));
    connect(colorByDomainsAction, SIGNAL(triggered()), this, SLOT(colorByDomains()));

    // 3.5.4
    colorByCOGsAction = new QAction(tr("By COGs"), this);
    colorByCOGsAction->setCheckable(true);
    colorByCOGsAction->setStatusTip(tr("Each COG is assigned to a specific color"));
    connect(colorByCOGsAction, SIGNAL(triggered()), this, SLOT(colorByCOGs()));

    // 3.6
    loadColorSchemeAction = new QAction(tr("Load Color Scheme..."), this);
    loadColorSchemeAction->setStatusTip(tr("Load a color scheme in a simple text format"));
    connect(loadColorSchemeAction, SIGNAL(triggered()), this, SLOT(loadColorScheme()));

    // 3.7
    colorArchitecturesAction = new QAction(tr("Color Architectures..."), this);
    colorArchitecturesAction->setStatusTip(tr("Load protein architectures in a simple text format to be colored"));
    connect(colorArchitecturesAction, SIGNAL(triggered()), this, SLOT(colorArchitectures()));

    //
    /*setPdfResolutionAction = new QAction(tr("Set PDF Resolution..."), this);
    setPdfResolutionAction->setStatusTip(tr("Set a resolution of PDF in points"));
    connect(setPdfResolutionAction, SIGNAL(triggered()), this, SLOT(setPdfResolution()));*/

    // 3.8
    setNeighborhoodRadiusAction = new QAction(tr("Set Neighborhood..."), this);
    setNeighborhoodRadiusAction->setStatusTip(tr("Set a number of genes in a neighborhood"));
    connect(setNeighborhoodRadiusAction, SIGNAL(triggered()), this, SLOT(setNeighborhoodRadius()));

    // 3.9
    setValuesAction = new QAction(tr("Advanced Settings..."), this);
    setValuesAction->setStatusTip(tr("Advanced settings"));
    connect(setValuesAction, SIGNAL(triggered()), this, SLOT(setValues()));

    // 4.1
    reconstructTreeAction = new QAction(tr("Reconstruct Tree..."), this);
    reconstructTreeAction->setStatusTip(tr("Reconstruct a phylogenetic tree of selected proteins"));
    connect(reconstructTreeAction, SIGNAL(triggered()), this, SLOT(reconstructTree()));

    // 4.2
    setPhylumColorAction = new QAction(tr("Set Phylum Color..."), this);
    setPhylumColorAction->setStatusTip(tr("Assign each phylum to a specific color"));
    connect(setPhylumColorAction, SIGNAL(triggered()), this, SLOT(setPhylumColor()));

    // 5.1
    aboutAction = new QAction(tr("About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // 3.5
    colorGenesGroup = new QActionGroup(this);
    colorGenesGroup->addAction(colorByCOGsAction);
    colorGenesGroup->addAction(colorByDomainsAction);
    colorGenesGroup->addAction(colorByCOGArchitecturesAction);
    colorGenesGroup->addAction(colorByDomainArchitecturesAction);
    colorByCOGsAction->setChecked(true);

    colorAction = new QAction(tr("Color"), this);
    connect(colorAction, SIGNAL(triggered()), this, SLOT(color()));

    decolorAction = new QAction(tr("Decolor"), this);
    connect(decolorAction, SIGNAL(triggered()), this, SLOT(decolor()));
}

void COGNAT::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("File")); // 1
    fileMenu->addAction(loadAction); // 1.1
    fileMenu->addAction(saveAction); // 1.2 ###
    //fileMenu->addAction(saveAsAction); // 1.3
    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentProjects; ++i)
        fileMenu->addAction(loadRecentProjectActions[i]); // 1.4
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction); // 1.5

    databaseMenu = menuBar()->addMenu(tr("Database")); // 2
    databaseMenu->addAction(searchGenesAction); // 2.1 ###
    databaseMenu->addSeparator();
    databaseMenu->addAction(specifyDatabaseAction); // 2.2

    optionsMenu = menuBar()->addMenu(tr("Options")); // 3
    optionsMenu->addAction(sortGenomesByTaxonomyAction); // 3.1 ###
    optionsMenu->addAction(markGenesOfMembraneProteinsAction); // 3.2 ###
    //optionsMenu->addAction(examineOrthologsAndParalogsAction); // 3.3
    optionsMenu->addAction(showIntergenesAction); // 3.4 ###
    colorGenesSubMenu = optionsMenu->addMenu(tr("Color Genes")); // 3.5
    colorGenesSubMenu->addAction(colorByCOGsAction); // 3.5.4
    colorGenesSubMenu->addAction(colorByDomainsAction); // 3.5.3
    colorGenesSubMenu->addAction(colorByCOGArchitecturesAction); // 3.5.2
    colorGenesSubMenu->addAction(colorByDomainArchitecturesAction); // 3.5.1
    optionsMenu->addSeparator();
    optionsMenu->addAction(loadColorSchemeAction); // 3.6
    optionsMenu->addAction(colorArchitecturesAction); // 3.7
    optionsMenu->addSeparator();
    //optionsMenu->addAction(setPdfResolutionAction); //
    optionsMenu->addAction(setNeighborhoodRadiusAction); // 3.8
    optionsMenu->addAction(setValuesAction); // 3.9

    //phylogenyMenu = menuBar()->addMenu(tr("Phylogeny")); // 4
    //phylogenyMenu->addAction(reconstructTreeAction); // 4.1
    //phylogenyMenu->addSeparator();
    //phylogenyMenu->addAction(setPhylumColorAction); // 4.2

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("Help")); // 5
    helpMenu->addAction(aboutAction); // 5.1
}

void COGNAT::createToolBars()
{
    colorGenesLabel = new QLabel(tr("Color Genes:"), this); // 3.5

    colorGenesComboBox = new QComboBox(this); // 3.5
    colorGenesComboBox->addItem(tr("By COGs")); // 3.5.4
    colorGenesComboBox->addItem(tr("By Domains")); // 3.5.3
    colorGenesComboBox->addItem(tr("By COG Architectures")); // 3.5.2
    colorGenesComboBox->addItem(tr("By Domain Architectures")); // 3.5.1
    colorGenesComboBox->setItemData(0, tr("Each COG is assigned to a specific color"), Qt::StatusTipRole); // 3.5.4
    colorGenesComboBox->setItemData(0, tr("Each COG is assigned to a specific color"), Qt::ToolTipRole); // 3.5.4
    colorGenesComboBox->setItemData(1, tr("Each domain is assigned to a specific color"), Qt::StatusTipRole); // 3.5.3
    colorGenesComboBox->setItemData(1, tr("Each domain is assigned to a specific color"), Qt::ToolTipRole); // 3.5.3
    colorGenesComboBox->setItemData(2, tr("Each COG architecture is assigned to a specific color"), Qt::StatusTipRole); // 3.5.2
    colorGenesComboBox->setItemData(2, tr("Each COG architecture is assigned to a specific color"), Qt::ToolTipRole); // 3.5.2
    colorGenesComboBox->setItemData(3, tr("Each domain architecture is assigned to a specific color"), Qt::StatusTipRole); // 3.5.1
    colorGenesComboBox->setItemData(3, tr("Each domain architecture is assigned to a specific color"), Qt::ToolTipRole); // 3.5.1
    colorGenesComboBox->setFocusPolicy(Qt::NoFocus); // 3.5
    connect(colorGenesGroup, SIGNAL(triggered(QAction *)), this, SLOT(setColorGenesComboBox(QAction *))); // 3.5
    connect(colorGenesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setColorGenesGroup(int))); // 3.5

    neighborhoodRadiusLabel = new QLabel(tr("Neighborhood:"), this); // 3.8
    neighborhoodRadiusLabel->setStatusTip(tr("Set a number of genes in a neighborhood")); // 3.8
    neighborhoodRadiusLabel->setToolTip(tr("Set a number of genes in a neighborhood")); // 3.8

    neighborhoodRadiusComboBox = new QComboBox(this); // 3.8
    for (int i = 1; i <= MaxNeighborhoodRadius; ++i) {
        if (i * 2 + 1 < 10) {
            neighborhoodRadiusComboBox->addItem(tr("%1 genes").arg(i * 2 + 1, 3));
        } else {
            neighborhoodRadiusComboBox->addItem(tr("%1 genes").arg(i * 2 + 1));
        }
    }
    neighborhoodRadiusComboBox->setCurrentIndex(neighborhoodRadius - 1); // 3.8
    neighborhoodRadiusComboBox->setFocusPolicy(Qt::NoFocus); // 3.8
    connect(neighborhoodRadiusComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modifyNeighborhoodRadius(int))); // 3.8

    fileToolBar = addToolBar(tr("File")); // 1
    fileToolBar->addAction(saveAction); // 1.2 ###
    fileToolBar->addSeparator();
    fileToolBar->setMovable(false);

    databaseToolBar = addToolBar(tr("Database")); // 2
    databaseToolBar->addAction(searchGenesAction); // 2.1 ###
    databaseToolBar->addSeparator();
    databaseToolBar->setMovable(false);

    optionsToolBar = addToolBar(tr("Options")); // 3
    optionsToolBar->addAction(sortGenomesByTaxonomyAction); // 3.1 ###
    optionsToolBar->addAction(markGenesOfMembraneProteinsAction); // 3.2 ###
    optionsToolBar->addAction(showIntergenesAction); // 3.4 ###
    optionsToolBar->addSeparator();
    optionsToolBar->addWidget(colorGenesLabel); // 3.5
    optionsToolBar->addWidget(colorGenesComboBox); // 3.5
    optionsToolBar->addSeparator();
    optionsToolBar->addWidget(neighborhoodRadiusLabel); // 3.8
    optionsToolBar->addWidget(neighborhoodRadiusComboBox); // 3.8
    optionsToolBar->setMovable(false);
}

void COGNAT::setGeometry()
{
    int x = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen()).width() / 10;
    int y = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen()).height() / 10;
    move(x, y);
    resize(x * 8, y * 8);
    setMinimumSize(sizeHint().width(), sizeHint().width() / 2);
}

void COGNAT::readSettings()
{
    /*QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    int width = printer.paperSize(QPrinter::Point).width();
    int height = printer.paperSize(QPrinter::Point).height();*/

    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    databasePath = settings.value("databasePath").toString();
    loadPath = settings.value("loadPath").toString();
    savePath = settings.value("savePath").toString();
    //pdfWidth = settings.value("pdfWidth", width).toInt();
    //pdfHeight = settings.value("pdfHeight", height).toInt();
    neighborhoodRadius = settings.value("neighborhoodRadius", 4).toInt();
    minimumBitScore = settings.value("minimumBitScore", 33).toInt();
    minimumRelativeLength = settings.value("minimumRelativeLength", 33).toInt();
    maximumOverlapping = settings.value("maximumOverlapping", 50).toInt();
    trustworthyBitScore = settings.value("trustworthyBitScore", 250).toInt();
    /*if (pdfWidth == 0) {
        pdfWidth = width;
    }
    if (pdfHeight == 0) {
        pdfHeight = height;
    }*/
    if (neighborhoodRadius == 0) {
        neighborhoodRadius = 4;
    }
    if (minimumBitScore == 0) {
        minimumBitScore = 33;
    }
    if (minimumRelativeLength == 0) {
        minimumRelativeLength = 33;
    }
    if (maximumOverlapping == 0) {
        maximumOverlapping = 50;
    }
    if (trustworthyBitScore == 0) {
        trustworthyBitScore = 250;
    }
    /*QSettings settings("Software Inc.", "Spreadsheet");

    restoreGeometry(settings.value("geometry").toByteArray());

    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();

    bool showGrid = settings.value("showGrid", true).toBool();
    showGridAction->setChecked(showGrid);

    bool autoRecalc = settings.value("autoRecalc", true).toBool();
    autoRecalcAction->setChecked(autoRecalc);*/
}

void COGNAT::writeSettings()
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    settings.setValue("databasePath", databasePath);
    settings.setValue("loadPath", loadPath);
    settings.setValue("savePath", savePath);
    //settings.setValue("pdfWidth", pdfWidth);
    //settings.setValue("pdfHeight", pdfHeight);
    settings.setValue("neighborhoodRadius", neighborhoodRadius);
    settings.setValue("minimumBitScore", minimumBitScore);
    settings.setValue("minimumRelativeLength", minimumRelativeLength);
    settings.setValue("maximumOverlapping", maximumOverlapping);
    settings.setValue("trustworthyBitScore", trustworthyBitScore);
    /*QSettings settings("Software Inc.", "Spreadsheet");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("showGrid", showGridAction->isChecked());
    settings.setValue("autoRecalc", autoRecalcAction->isChecked());*/
}

void COGNAT::writeColorToSettings(const QString &architecture, const QColor &color)
{
    QSettings settings("Molecular Bioenergetics Group", "COGNAT");
    settings.setValue(architecture, color);
}

bool COGNAT::okToContinue()
{
    /*if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Spreadsheet"),
                        tr("The document has been modified.\n"
                           "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::No
                        | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }*/
    return true;
}

void COGNAT::loadTextFile(const QString &fileName)
{
    inputType = Default;
    if (mainWidget != NULL) {
        delete mainWidget;
        mainWidget = NULL;
    }
    QString name = QFileInfo(fileName).fileName();
    if (database->loadTextFile(fileName)) { // The database was cleared.
        //setCurrentFile(fileName); ???
        statusBar()->showMessage(tr("A text file '%1' was loaded")
                                 .arg(name));
        mainWidget = new MainWidget(database, this); // Only a "skeleton" of an output window was created.
        mainWidget->setRadius(neighborhoodRadius);
        mainWidget->setMinimumScore(minimumBitScore);
        mainWidget->setMinimumLength(minimumRelativeLength);
        mainWidget->setMaximumOverlapping(maximumOverlapping);
        mainWidget->setTrustworthyScore(trustworthyBitScore);
        setCentralWidget(mainWidget);
        sortGenomesByTaxonomy(sortGenomesByTaxonomyAction->isChecked());
    } else {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not load a text file '%1'.\n\n%2.")
                             .arg(name).arg(database->errorString()));
    }
}

void COGNAT::loadSearchedGenes(const QString &fileName)
{
    inputType = Default;
    if (mainWidget != NULL) {
        delete mainWidget;
        mainWidget = NULL;
    }
    QString name = QFileInfo(fileName).fileName();
    if (database->loadTextFile(fileName)) { // The database was cleared.
        //setCurrentFile(fileName); ???
        statusBar()->showMessage(tr("Searched genes were loaded"));
        mainWidget = new MainWidget(database, this); // Only a "skeleton" of an output window was created.
        mainWidget->setRadius(neighborhoodRadius);
        mainWidget->setMinimumScore(minimumBitScore);
        mainWidget->setMinimumLength(minimumRelativeLength);
        mainWidget->setMaximumOverlapping(maximumOverlapping);
        mainWidget->setTrustworthyScore(trustworthyBitScore);
        setCentralWidget(mainWidget);
        sortGenomesByTaxonomy(sortGenomesByTaxonomyAction->isChecked());
    } else {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not search genes.\n\n%1.")
                             .arg(database->errorString()));
    }
}

void COGNAT::loadProject(const QString &fileName)
{
    //setCurrentFile(fileName); ???
    /*QString name = QFileInfo(fileName).fileName();
    statusBar()->showMessage(tr("A project '%1' was loaded")
                             .arg(name));*/
    //inputType = {one of 3} ???
    QMessageBox::warning(this, tr("Warning"),
                         tr("Sorry, this option is currently unavailable."));
}

void COGNAT::savePdf(const QString &fileName)
{
    QString name = QFileInfo(fileName).fileName();
    //QSizeF size(pdfWidth, pdfHeight);
    //if (mainWidget->savePdf(fileName, size)) {
    if (mainWidget->savePdf(fileName)) {
        savePath = QFileInfo(fileName).canonicalPath();
        //setCurrentFile(fileName); ???
        statusBar()->showMessage(tr("PDF '%1' was saved")
                                 .arg(name));
    } else {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Could not save PDF '%1'.")
                             .arg(name));
    }
}

void COGNAT::saveProject(const QString &fileName)
{
    //setCurrentFile(fileName); ???
    /*QString name = QFileInfo(fileName).fileName();
    statusBar()->showMessage(tr("A project '%1' was saved")
                             .arg(name));*/
    QMessageBox::warning(this, tr("Warning"),
                         tr("Sorry, this option is currently unavailable."));
}

/*void COGNAT::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = tr("Untitled");
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        updateRecentFileActions();
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("Spreadsheet")));
}

void COGNAT::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = tr("&%1 %2")
                           .arg(j + 1)
                           .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        } else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}*/