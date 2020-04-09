#include <QApplication>
#include <QChar>
#include <QCursor>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include "database.h"

Database::Database(const QString &path, const int &maxradius, QWidget *parent)
    : QDir(path), QProgressDialog(parent)
{
    textFileLoadingResult = false;
    membraneAnnotationLoadingResult = false;
    domainAnnotationLoadingResult = false;
    COGAnnotationLoadingResult = false;
    domainDescriptionLoadingResult = false;
    COGDescriptionLoadingResult = false;
    maxRadius = maxradius;
}

void Database::updateAnchors(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, AnnotationType annotationType)
{
    foreach (const QString &gi, architectureMap.keys()) {
        if (annotationType == Domains) {
            if (proteinMap.contains(gi)) {
                QString architecture = proteinMap[gi].getDomainArchitecture(minscore, minlength, maxoverlap, trustscore);
                if (!gi.isEmpty() && !architecture.isEmpty()) {
                    architectureMap.remove(gi);
                    architectureMap.insert(gi, architecture);
                }
            } // A necessary check, otherwise a default value will be inserted into a map.
        } else if (annotationType == COGs) {
            if (proteinMap.contains(gi)) {
                QString architecture = proteinMap[gi].getCOGArchitecture(minscore, minlength, maxoverlap, trustscore);
                if (!gi.isEmpty() && !architecture.isEmpty()) {
                    architectureMap.remove(gi);
                    architectureMap.insert(gi, architecture);
                }
            } // A necessary check, otherwise a default value will be inserted into a map.
        }
    }
}

void Database::insertAnchor(const QString &gi, const QString &architecture)
{
    if (!gi.isEmpty() && !architecture.isEmpty()) {
        architectureMap.insert(gi, architecture);
    }
}

void Database::removeAnchor(const QString &architecture)
{
    foreach (const QString &gi, architectureMap.keys()) {
        if (architectureMap[gi] == architecture) {
            architectureMap.remove(gi);
        }
    }
    architectureList.removeAll(architecture);
    removedArchitecture = architecture;
}

void Database::setExamining(const QString &gi, bool checked)
{
    if (proteinMap.contains(gi)) {
        proteinMap[gi].setExamining(checked);
    } // A necessary check, otherwise a default value will be inserted into a map.
}

void Database::loadArchitectures(const QString &architectures)
{
    architectureList.clear();

    QStringList list = architectures.split("\n");
    foreach (const QString &line, list) {
        QString architecture = line.section("\t", 0, 0).trimmed();
        if (!architecture.isEmpty()) {
            architectureList.append(architecture);
        }
    }
}

bool Database::contains(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return true;
    }
    return false;
}

bool Database::isExamining(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].isExamining();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return false;
}

bool Database::isValid() const
{
    QDirIterator iterator(canonicalPath(), AllDirs | NoDotAndDotDot, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QDir dir(iterator.next());
        QString accession = dir.dirName();
        //QStringList required = annotationOf(accession);
        QStringList required;
        required.append(QString("a_%1.gis").arg(accession));
        required.append(QString("a_%1.ids").arg(accession));
        required.append(QString("c_%1.txt").arg(accession));
        required.append(QString("d_%1.txt").arg(accession));
        required.append(QString("g_%1.fasta").arg(accession));
        required.append(QString("m_%1.txt").arg(accession));
        required.append(QString("p_%1.fasta").arg(accession));
        QStringList existing = dir.entryList(Files);
        if (existing == required) {
            return true; // At least one accession directory is valid.
        }
    }
    return false;
}

bool Database::loadTextFile(const QString &fileName)
{
    inputSet.clear();
    inputVector.clear();
    addressMap.clear();
    proteinMap.clear();
    gbkMap.clear();
    architectureMap.clear();
    domainDescriptionMap.clear();
    COGDescriptionMap.clear();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
#ifndef Q_OS_MAC
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    gis = false;
    ids = false;
    textFileLoadingResult = false;
    membraneAnnotationLoadingResult = false;
    domainAnnotationLoadingResult = false;
    COGAnnotationLoadingResult = false;
    domainDescriptionLoadingResult = false;
    COGDescriptionLoadingResult = false;

    if (!okTextFile(fileName)) {
        return false;
    }
    if (!gis && !ids) {
        errorDescription = tr("Wrong file format");
        return false;
    } // else { Here are a non-empty SET and a non-empty VECTOR of gis and/or ids to load. }

    QDirIterator iterator(canonicalPath(), AllDirs | NoDotAndDotDot, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QDir dir(iterator.next());
        QString accession = dir.dirName();
        //QStringList required = annotationOf(accession);
        QStringList required;
        required.append(QString("a_%1.gis").arg(accession));
        required.append(QString("a_%1.ids").arg(accession));
        required.append(QString("c_%1.txt").arg(accession));
        required.append(QString("d_%1.txt").arg(accession));
        required.append(QString("g_%1.fasta").arg(accession));
        required.append(QString("m_%1.txt").arg(accession));
        required.append(QString("p_%1.fasta").arg(accession));
        QStringList existing = dir.entryList(Files);
        if (existing == required) {
            QString dirPath = dir.canonicalPath();
            allDirPaths.insert(dirPath);
        }
    } // Here is a non-empty SET of paths of accession directories.
    int counter = 0; // progress
    int size = allDirPaths.size() + inputSet.size(); // progress
    setRange(0, size); // progress
    setCancelButtonText(QString()); // progress
    setFixedSize(sizeHint().width(), sizeHint().height()); // progress
    setLabelText(tr("Loading...")); // progress
    setModal(true); // progress
    forceShow(); // progress

    foreach (const QString &dirPath, allDirPaths) {
        QDir dir(dirPath);
        QString accession = dir.dirName();
        if (gis) {
            QString aFileName = dir.filePath(QString("a_%1.gis").arg(accession));
            if (!okAFile(aFileName)) {
                reset(); // progress
                return false;
            }
        }
        if (ids) {
            QString aFileName = dir.filePath(QString("a_%1.ids").arg(accession));
            if (!okAFile(aFileName)) {
                reset(); // progress
                return false;
            }
        }
        ++counter; // progress
        setValue(counter); // progress
    }
    if (addressMap.isEmpty()) {
        errorDescription = tr("Nothing found in the database");
        reset(); // progress
        return false;
    } // else { Here is a non-empty MAP converting a directory path to line numbers of a protein FASTA file. }
    allDirPaths.clear();

    foreach (const QString &dirPath, addressMap.keys()) {
        QDir dir(dirPath);
        QString accession = dir.dirName();
        QString pFileName = dir.filePath(QString("p_%1.fasta").arg(accession));
        if (!okPFile(pFileName)) {
            reset(); // progress
            return false;
        } // else { Here are two non-empty MAPs: "accession-Gbk" ang "gi-Protein". Input updated to gis. }
        QString gFileName = dir.filePath(QString("g_%1.fasta").arg(accession));
        if (!okGFile(gFileName)) {
            reset(); // progress
            return false;
        } // else { A non-empty MAP "accession-Gbk" updated in the fields: "circular", "complete", "source", "taxonomy", "definition". }
        ++counter; // progress
        setValue(counter); // progress
    }

    setValue(size); // progress
    reset(); // progress
#ifndef Q_OS_MAC
    qApp->restoreOverrideCursor();
#endif

    textFileLoadingResult = true;

    return true;
}

bool Database::textFileWasLoaded() const
{
    return textFileLoadingResult;
}

bool Database::textFileWasWritten(const QString &domainAccession, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const QString &fileName)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
#ifndef Q_OS_MAC
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    QSet<QString> tmpAllDirPaths;
    QDirIterator iterator(canonicalPath(), AllDirs | NoDotAndDotDot, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QDir dir(iterator.next());
        QString accession = dir.dirName();
        //QStringList required = annotationOf(accession);
        QStringList required;
        required.append(QString("a_%1.gis").arg(accession));
        required.append(QString("a_%1.ids").arg(accession));
        required.append(QString("c_%1.txt").arg(accession));
        required.append(QString("d_%1.txt").arg(accession));
        required.append(QString("g_%1.fasta").arg(accession));
        required.append(QString("m_%1.txt").arg(accession));
        required.append(QString("p_%1.fasta").arg(accession));
        QStringList existing = dir.entryList(Files);
        if (existing == required) {
            QString dirPath = dir.canonicalPath();
            tmpAllDirPaths.insert(dirPath);
        }
    } // Here is a non-empty SET of paths of accession directories.
    int counter = 0; // progress
    int size = tmpAllDirPaths.size(); // progress
    setRange(0, size); // progress
    setCancelButtonText(QString()); // progress
    setFixedSize(sizeHint().width(), sizeHint().height()); // progress
    setLabelText(tr("Searching...")); // progress
    setModal(true); // progress
    forceShow(); // progress

    foreach (const QString &dirPath, tmpAllDirPaths) {
        QDir dir(dirPath);
        QString accession = dir.dirName();
        QString dFileName;
        if (domainAccession.startsWith("COG")) {
            dFileName = dir.filePath(QString("c_%1.txt").arg(accession));
        } else {
            dFileName = dir.filePath(QString("d_%1.txt").arg(accession));
        }
        if (!okAppendingToTextFile(domainAccession, minscore, minlength, maxoverlap, trustscore, dFileName, fileName)) {
            reset(); // progress
            return false;
        }
        ++counter; // progress
        setValue(counter); // progress
    }
    QFile file(fileName);
    if (file.size() == 0) {
        file.remove();
        errorDescription = tr("Nothing found in the database");
        reset(); // progress
        return false;
    } // else { Here is a non-empty FILE of gis to load. }

    setValue(size); // progress
    reset(); // progress
#ifndef Q_OS_MAC
    qApp->restoreOverrideCursor();
#endif

    return true;
}

bool Database::loadAnnotation(AnnotationType annotationType)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
#ifndef Q_OS_MAC
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    int counter = 0; // progress
    int size = addressMap.keys().size(); // progress
    setRange(0, size); // progress
    setCancelButtonText(QString()); // progress
    setFixedSize(sizeHint().width(), sizeHint().height()); // progress
    if (annotationType == Membrane) {
        setLabelText(tr("Hatching...")); // progress
    } else if (annotationType == Domains) {
        setLabelText(tr("Coloring...")); // progress
    } else if (annotationType == COGs) {
        setLabelText(tr("Coloring...")); // progress
    }
    setModal(true); // progress
    forceShow(); // progress

    foreach (const QString &dirPath, addressMap.keys()) {
        QDir dir(dirPath);
        QString accession = dir.dirName();
        if (annotationType == Membrane) {
            QString mFileName = dir.filePath(QString("m_%1.txt").arg(accession));
            if (!okMFile(mFileName)) {
                reset(); // progress
                return false;
            } // else { A non-empty MAP "gi-Protein" updated in the field "helices". }
        } else {
            QString dFileName;
            if (annotationType == Domains) {
                dFileName = dir.filePath(QString("d_%1.txt").arg(accession));
            } else if (annotationType == COGs) {
                dFileName = dir.filePath(QString("c_%1.txt").arg(accession));
            }
            if (!okDFile(dFileName, annotationType)) {
                reset(); // progress
                return false;
            } // else { A non-empty MAP "gi-Protein" updated in the field "domains" or "COGs". }
        }
        ++counter; // progress
        setValue(counter); // progress
    }

    QString descriptionfileName;
    if (annotationType == Domains) {
        descriptionfileName = filePath(QString("description_Pfam.txt"));
    } else if (annotationType == COGs) {
        descriptionfileName = filePath(QString("description_COG.txt"));
    }
    if (exists(descriptionfileName)) {
        loadDescription(descriptionfileName, annotationType);
    }

    setValue(size); // progress
    reset(); // progress
#ifndef Q_OS_MAC
    qApp->restoreOverrideCursor();
#endif

    if (annotationType == Membrane) {
        membraneAnnotationLoadingResult = true;
    } else if (annotationType == Domains) {
        domainAnnotationLoadingResult = true;
    } else if (annotationType == COGs) {
        COGAnnotationLoadingResult = true;
    }

    return true;
}

bool Database::annotationWasLoaded(AnnotationType annotationType) const
{
    if (annotationType == Membrane) {
        return membraneAnnotationLoadingResult;
    } else if (annotationType == Domains) {
        return domainAnnotationLoadingResult;
    } else if (annotationType == COGs) {
        return COGAnnotationLoadingResult;
    }
    return false;
}

bool Database::descriptionWasLoaded(AnnotationType annotationType) const
{
    if (annotationType == Domains) {
        return domainDescriptionLoadingResult;
    } else if (annotationType == COGs) {
        return COGDescriptionLoadingResult;
    }
    return false;
}

bool Database::isMembrane(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].isMembrane();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return false;
}

int Database::getMaxRadius() const
{
    return maxRadius;
}

int Database::strandOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getStrand().toInt();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return 0;
}

int Database::startOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getStart();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return - 1;
}

int Database::endOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getEnd();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return - 1;
}

int Database::geneLengthOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getGeneLength();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return 0;
}

int Database::proteinLengthOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getLength();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return 0;
}

int Database::helicesOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getHelices();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return 0;
}

QString Database::errorString() const
{
    return errorDescription;
}

QString Database::lastRemovedArchitecture() const
{
    return removedArchitecture;
}

QString Database::domainOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getDomain();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::phylumOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getPhylum();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::classOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getClass();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::orderOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getOrder();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::familyOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getFamily();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::genusOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getGenus();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::sourceOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getSource();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::definitionOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].getDefinition();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::accessionOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getAccession();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::idOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getId();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::locusOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getLocus();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::descriptionOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getDescription();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::descriptionOf(const QString &domainAccession, AnnotationType annotationType) const
{
    if (annotationType == Domains) {
        if (domainDescriptionMap.contains(domainAccession)) {
            return domainDescriptionMap.value(domainAccession);
        } // A necessary check, otherwise a default value will be inserted into a map.
    } else if (annotationType == COGs) {
        if (COGDescriptionMap.contains(domainAccession)) {
            return COGDescriptionMap.value(domainAccession);
        } // A necessary check, otherwise a default value will be inserted into a map.
    }
    return QString();
}

QString Database::frameOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getFrame();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::coordinatesOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getCoordinates();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::proteinSequenceOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getSequence();
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::nucleotideSequenceOf(const QString &gi) const
{
    QString nucleotideSequence;
    QString coordinates;
    QString accession;
    QString gFileName;

    if (proteinMap.contains(gi)) {
        coordinates = proteinMap[gi].getCoordinates();
        accession = proteinMap[gi].getAccession();
    } // A necessary check, otherwise a default value will be inserted into a map.

    if (!coordinates.isEmpty() && !accession.isEmpty()) {
        foreach (const QString &dirPath, addressMap.keys()) {
            QStringList accession_parts = accession.split(".", QString::SkipEmptyParts);
            QString accession_first_part = accession_parts.at(0);
            if (dirPath.endsWith(accession_first_part)) {
                QDir dir(dirPath);
                gFileName = dir.filePath(QString("g_%1.fasta").arg(accession_first_part));
                break;
            }
        }
    }

    if (!gFileName.isEmpty()) {
        QFile file(gFileName);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            QStringList exons = coordinates.split(",");
            QString line = in.readLine();
            if (proteinMap[gi].getStrand().toInt() > 0) {
                for (int i = 0; i < exons.size(); ++i) {
                    int start = trimmedCoordinate(exons.value(i).section("..", 0, 0));
                    int end = trimmedCoordinate(exons.value(i).section("..", 1, 1));
                    if (end == 0) {
                        end = start;
                    }
                    in.seek(line.size() + start);
                    nucleotideSequence += in.read(end - start + 1);
                    in.seek(0);
                }
            } else if (proteinMap[gi].getStrand().toInt() < 0) {
                for (int i = exons.size() - 1; i > - 1; --i) {
                    int start = trimmedCoordinate(exons.value(i).section("..", 0, 0));
                    int end = trimmedCoordinate(exons.value(i).section("..", 1, 1));
                    if (end == 0) {
                        end = start;
                    }
                    in.seek(line.size() + start);
                    nucleotideSequence += reverseComplementTo(in.read(end - start + 1));
                    in.seek(0);
                }
            }
        }
    }

    return nucleotideSequence;
}

QString Database::forwardSequenceOf(const QString &gi, const int &start, const int &end) const
{
    QString nucleotideSequence;
    QString accession;
    QString gFileName;

    if (proteinMap.contains(gi)) {
        accession = proteinMap[gi].getAccession();
    } // A necessary check, otherwise a default value will be inserted into a map.

    if (!accession.isEmpty()) {
        foreach (const QString &dirPath, addressMap.keys()) {
            QStringList accession_parts = accession.split(".", QString::SkipEmptyParts);
            QString accession_first_part = accession_parts.at(0);
            if (dirPath.endsWith(accession_first_part)) {
                QDir dir(dirPath);
                gFileName = dir.filePath(QString("g_%1.fasta").arg(accession_first_part));
                break;
            }
        }
    }

    if (!gFileName.isEmpty()) {
        QFile file(gFileName);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            QString line = in.readLine();
            if (start > end) {
                in.seek(line.size() + start);
                while (!in.atEnd()) {
                    nucleotideSequence += in.read(1).trimmed();
                }
                in.seek(0);
                in.seek(line.size() + 1);
                nucleotideSequence += in.read(end);
            } else {
                in.seek(line.size() + start);
                nucleotideSequence = in.read(end - start + 1);
            }
        }
    }

    return nucleotideSequence;
}

QString Database::reverseComplementTo(const QString &string) const
{
    QString reverseComplement;
    for (int i = string.size() - 1; i > - 1; --i) {
        if (string.at(i) == QChar('a')) {
            reverseComplement += QString("t");
        } else if (string.at(i) == QChar('t')) {
            reverseComplement += QString("a");
        } else if (string.at(i) == QChar('g')) {
            reverseComplement += QString("c");
        } else if (string.at(i) == QChar('c')) {
            reverseComplement += QString("g");
        } else {
            reverseComplement += QString("n");
        }
    }
    return reverseComplement;
}

QString Database::domainArchitectureOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getDomainArchitecture(minscore, minlength, maxoverlap, trustscore);
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QString Database::COGArchitectureOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getCOGArchitecture(minscore, minlength, maxoverlap, trustscore);
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QString();
}

QSet<QString> Database::getInputSet() const
{
    return inputSet; // The set either is empty, or contains input gis.
}

QVector<QString> Database::getInputVector() const
{
    return inputVector; // The vector either is empty, or contains input gis.
}

QVector<QString> Database::neighborhoodOf(const QString &gi, const int &radius) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].neighborhoodOf(gi, radius);
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QVector<QString>();
}

QVector<QString> Database::maxNeighborhoodOf(const QString &gi) const
{
    if (proteinMap.contains(gi)) {
        QString accession = proteinMap[gi].getAccession();
        return gbkMap[accession].neighborhoodOf(gi, maxRadius);
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QVector<QString>();
}

QList<QString> Database::getArchitectureList() const
{
    return architectureMap.values() + architectureList; // The list either is empty, or contains examining architectures.
}

QList<Domain> Database::domainsOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getDomains(minscore, minlength, maxoverlap, trustscore);
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QList<Domain>();
}

QList<Domain> Database::COGsOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const
{
    if (proteinMap.contains(gi)) {
        return proteinMap[gi].getCOGs(minscore, minlength, maxoverlap, trustscore);
    } // A necessary check, otherwise a default value will be inserted into a map.
    return QList<Domain>();
}

void Database::updateInput(const Protein &protein)
{
    QString gi = protein.getGi();
    QString id = protein.getId().section(".", 0, 0);
    if (inputSet.remove(id)) {
        inputSet.insert(gi);
        int number = inputVector.count(id);
        for (int i = 0; i < number; ++i) {
            int index = inputVector.indexOf(id);
            inputVector.replace(index, gi);
        }
    }
}

void Database::loadDescription(const QString &fileName, AnnotationType annotationType)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QString domainAccession = line.section("\t", 0, 0);
        QString domainDescription = line.section("\t", 1, 1);
        if (!domainAccession.isEmpty() && !domainDescription.isEmpty()) {
            if (annotationType == Domains) {
                domainDescriptionMap.insert(domainAccession, domainDescription);
            } else if (annotationType == COGs) {
                COGDescriptionMap.insert(domainAccession, domainDescription);
            }
        }
    }

    if (!domainDescriptionMap.isEmpty()) {
        domainDescriptionLoadingResult = true;
    }
    if (!COGDescriptionMap.isEmpty()) {
        COGDescriptionLoadingResult = true;
    }
}

bool Database::okTextFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = file.errorString();
        return false;
    }

    QRegExp gi("[0-9]+");
    //QRegExp id("[A-Z]{2}\\_[0-9]+\\.?[0-9]?");
    QRegExp id("[A-Z0-9_]+.?[0-9]?");
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        line = line.trimmed();
        if (gi.exactMatch(line)) {
            gis = true;
            inputSet.insert(line);
            inputVector.append(line);
        } else if (id.exactMatch(line)) {
            ids = true;
            inputSet.insert(line.section(".", 0, 0));
            inputVector.append(line.section(".", 0, 0));
        } else {
            if (!line.isEmpty()) {
                inputSet.insert(line);
                inputVector.append(line);
            }
        }
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    return true;
}

bool Database::okAFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = tr("Database error");
        return false;
    }

    int counter = 0;
    QSet<int> tmp;
    QTextStream in(&file);
    while (!in.atEnd()) {
        ++counter;
        QString line = in.readLine();
        line = line.trimmed();
        if (inputSet.contains(line.section(".", 0, 0))) {
            tmp.insert(counter);
        }
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    if (!tmp.isEmpty()) {
        QString dirPath = QFileInfo(fileName).canonicalPath();
        if (!addressMap.contains(dirPath)) {
            addressMap.insert(dirPath, QSet<int>()); // A prediction of line numbers of a protein FASTA file.
        }
        foreach (const int &number, tmp) {
            int baseline1 = (number - 1) * 3 + 1;
            int baseline2 = (number - 1) * 3 + 2;
            addressMap[dirPath].insert(baseline1);
            addressMap[dirPath].insert(baseline2);
            for (int i = 1; i <= maxRadius; ++i) {
                int prevline1 = (number - 1 - i) * 3 + 1;
                int prevline2 = (number - 1 - i) * 3 + 2;
                if (prevline1 > 0) {
                    addressMap[dirPath].insert(prevline1);
                    addressMap[dirPath].insert(prevline2);
                } else {
                    addressMap[dirPath].insert(prevline1 + counter * 3);
                    addressMap[dirPath].insert(prevline2 + counter * 3);
                }
                int nextline1 = (number - 1 + i) * 3 + 1;
                int nextline2 = (number - 1 + i) * 3 + 2;
                if (nextline2 < counter * 3) {
                    addressMap[dirPath].insert(nextline1);
                    addressMap[dirPath].insert(nextline2);
                } else {
                    addressMap[dirPath].insert(nextline1 - counter * 3);
                    addressMap[dirPath].insert(nextline2 - counter * 3);
                }
            }
        }
    }

    return true;
}

bool Database::okPFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = tr("Database error");
        return false;
    }

    int counter = 0;
    QString dirPath = QFileInfo(fileName).canonicalPath();
    QStringList fields;
    QTextStream in(&file);
    QVector<QString> vector;
    while (!in.atEnd()) {
        ++counter;
        QString line = in.readLine();
        if (addressMap[dirPath].contains(counter)) {
            if (line.startsWith(">")) {
                fields = line.split("|");
            } else {
                Protein protein(fields, line);
                QString gi = protein.getGi();
                proteinMap.insert(gi, protein);
                vector.append(gi);
                if (ids) {
                    updateInput(protein);
                } // Input updated to gis.
                if (inputSet.contains(gi)) {
                    proteinMap[gi].setExamining(true);
                }
            }
        }
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    QString gi = vector.value(0);
    QString accession = proteinMap[gi].getAccession();
    Gbk gbk(accession, vector);
    gbkMap.insert(accession, gbk);

    return true;
}

bool Database::okGFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = tr("Database error");
        return false;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList fields = line.split("|");
    QString accession = fields.value(1);
    gbkMap[accession].setSource(fields.value(2));
    gbkMap[accession].setTaxonomy(fields.value(3));
    gbkMap[accession].setDefinition(fields.value(4));
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    return true;
}

bool Database::okMFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = tr("Database error");
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QString gi = line.section("\t", 0, 0);
        if (proteinMap.contains(gi)) {
            QString number = line.section("\t", 1, 1);
            int helices_number = number.toInt();
            if (helices_number > 1) {
                proteinMap[gi].setHelices(number);
            }
        } // A necessary check, otherwise a default value will be inserted into a map.
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    return true;
}

bool Database::okDFile(const QString &fileName, AnnotationType annotationType)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = tr("Database error");
        return false;
    }

    QMap< QString, QVector<Domain> > tmp;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QString gi = line.section("\t", 0, 0);
        if (proteinMap.contains(gi)) {
            Domain domain(line);
            if (!tmp.contains(gi)) {
                tmp.insert(gi, QVector<Domain>());
            }
            tmp[gi].append(domain);
        } // A necessary check, otherwise a default value will be inserted into a map.
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    foreach (const QString &gi, tmp.keys()) {
        if (annotationType == Domains) {
            proteinMap[gi].setDomains(tmp.value(gi));
        } else if (annotationType == COGs) {
            proteinMap[gi].setCOGs(tmp.value(gi));
        }
    }

    return true;
}

bool Database::okAppendingToTextFile(const QString &domainAccession, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const QString &fileName, const QString &textFileName)
{
    QMap< QString, QVector<Domain> > tmp;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        errorDescription = tr("Database error");
        return false;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QString accession = line.section("\t", 1, 1);
        if (accession == domainAccession) {
            Domain domain(line);
            if (domain.getBitScore() >= minscore && domain.getRelativeLength() >= minlength) {
                QString gi = line.section("\t", 0, 0);
                if (!tmp.contains(gi)) {
                    tmp.insert(gi, QVector<Domain>());
                }
            }
        }
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    if (tmp.keys().isEmpty()) {
        return true;
    }

    in.seek(0);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QString gi = line.section("\t", 0, 0);
        if (tmp.contains(gi)) {
            Domain domain(line);
            tmp[gi].append(domain);
        }
    }
    if (in.status() != QTextStream::Ok) {
        errorDescription = tr("Something bad happened");
        return false;
    }

    QFile textFile(textFileName);
    textFile.open(QIODevice::Append);
    QTextStream out(&textFile);
    foreach (const QString &gi, tmp.keys()) {
        QVector<Domain> domains = tmp[gi];
        int j = 0;
        QList<Domain> architecture;
        foreach (const Domain &domain, domains) {
            bool valid = false;
            if (domain.getBitScore() >= minscore && domain.getRelativeLength() >= minlength) {
                valid = true;
                for (int i = 0; i < domains.size(); ++i) {
                    if (i == j) {
                        continue;
                    }
                    Domain neighbor = domains.value(i);
                    if (domain.getStart() <= neighbor.getEnd() && domain.getEnd() >= neighbor.getStart()) { // Domains overlap.
                        valid = false;
                        if (domain.getBitScore() >= neighbor.getBitScore()) {
                            valid = true;
                        } else {
                            QList<int> list;
                            list << domain.getStart() << domain.getEnd()
                            << neighbor.getStart() << neighbor.getEnd();
                            qSort(list.begin(), list.end());
                            int overlapLength = list.value(2) - list.value(1) + 1;
                            int percent = overlapLength * 100 / domain.getLength();
                            if (percent <= maxoverlap) {
                                valid = true;
                            } else if (domain.getBitScore() >= trustscore) {
                                valid = true;
                            }
                        }
                    }
                    if (!valid) {
                        break;
                    }
                }
            }
            if (valid) {
                bool inserted = false;
                int i = 0;
                QList<Domain>::iterator iterator;
                for (iterator = architecture.begin(); iterator != architecture.end(); ++iterator) {
                    if (architecture.value(i).getStart() > domain.getStart()) {
                        architecture.insert(iterator, domain);
                        inserted = true;
                        break;
                    }
                    ++i;
                }
                if (!inserted) {
                    architecture.append(domain);
                }
            }
            ++j;
        }
        bool searched = false;
        foreach (const Domain &domain, architecture) {
            if (domain.getAccession() == domainAccession) {
                searched = true;
            }
        }
        if (searched) {
            out << gi << endl;
        }
    }

    return true;
}

int Database::trimmedCoordinate(const QString &string) const
{
    QString coordinate = string;
    coordinate.remove(QChar('<'));
    coordinate.remove(QChar('>'));
    return coordinate.toInt();
}

/*QStringList Database::annotationOf(const QString &accession) const
{
    QStringList names;
    names.append(QString("a_%1.gis").arg(accession));
    names.append(QString("a_%1.ids").arg(accession));
    names.append(QString("c_%1.txt").arg(accession));
    names.append(QString("d_%1.txt").arg(accession));
    names.append(QString("g_%1.fasta").arg(accession));
    names.append(QString("m_%1.txt").arg(accession));
    names.append(QString("p_%1.fasta").arg(accession));
    names.append(QString("p_%1.phr").arg(accession));
    names.append(QString("p_%1.pin").arg(accession));
    names.append(QString("p_%1.psq").arg(accession));
    return names;
}*/
