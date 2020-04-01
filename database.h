#ifndef DATABASE_H
#define DATABASE_H

#define DATABASE_NAME "COGNATDatabase"

#include <QDir>
#include <QList>
#include <QMap>
#include <QProgressDialog>
#include <QSet>
#include <QString>
//#include <QStringList>
#include <QVector>
#include "domain.h"
#include "gbk.h"
#include "protein.h"

class Database : public QDir, public QProgressDialog
{
public:
    Database(const QString &path = QString(), const int &maxradius = 0, QWidget *parent = NULL);

    enum AnnotationType { Membrane, Domains, COGs };

    AnnotationType annotationType;

    void updateAnchors(const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, AnnotationType annotationType);
    void insertAnchor(const QString &gi, const QString &architecture);
    void removeAnchor(const QString &architecture);
    void setExamining(const QString &gi, bool checked);
    void loadArchitectures(const QString &architectures);

    bool contains(const QString &gi) const; // by default = false
    bool isExamining(const QString &gi) const; // by default = false
    bool isValid() const;
    bool loadTextFile(const QString &fileName);
    bool textFileWasLoaded() const; // by default = false
    bool textFileWasWritten(const QString &domainAccession, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const QString &fileName);
    bool loadAnnotation(AnnotationType annotationType);
    bool annotationWasLoaded(AnnotationType annotationType) const; // by default = false
    bool descriptionWasLoaded(AnnotationType annotationType) const; // by default = false
    bool isMembrane(const QString &gi) const; // by default = false

    int getMaxRadius() const;
    int strandOf(const QString &gi) const; // by default = 0
    int startOf(const QString &gi) const; // by default = - 1
    int endOf(const QString &gi) const; // by default = - 1
    int geneLengthOf(const QString &gi) const; // by default = 0
    int proteinLengthOf(const QString &gi) const; // by default = 0
    int helicesOf(const QString &gi) const; // by default = 0

    QString errorString() const; // Not const &?
    QString lastRemovedArchitecture() const; // Not const &?
    QString domainOf(const QString &gi) const; // Not const &! (a local variable in Gbk)
    QString phylumOf(const QString &gi) const; // Not const &! (a local variable in Gbk)
    QString classOf(const QString &gi) const; // Not const &! (a local variable in Gbk)
    QString orderOf(const QString &gi) const; // Not const &! (a local variable in Gbk)
    QString familyOf(const QString &gi) const; // Not const &! (a local variable in Gbk)
    QString genusOf(const QString &gi) const; // Not const &! (a local variable in Gbk)
    QString sourceOf(const QString &gi) const;
    QString definitionOf(const QString &gi) const;
    QString accessionOf(const QString &gi) const;
    QString idOf(const QString &gi) const;
    QString locusOf(const QString &gi) const;
    QString descriptionOf(const QString &gi) const;
    QString descriptionOf(const QString &domainAccession, AnnotationType annotationType) const;
    QString frameOf(const QString &gi) const;
    QString coordinatesOf(const QString &gi) const;
    QString proteinSequenceOf(const QString &gi) const;
    QString nucleotideSequenceOf(const QString &gi) const;
    QString forwardSequenceOf(const QString &gi, const int &start, const int &end) const;
    QString reverseComplementTo(const QString &string) const;
    QString domainArchitectureOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // Not const &! (a local variable in Protein)
    QString COGArchitectureOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // Not const &! (a local variable in Protein)

    QSet<QString> getInputSet() const;

    QVector<QString> getInputVector() const;
    QVector<QString> neighborhoodOf(const QString &gi, const int &radius) const; // Not const &! (a local variable in Gbk)
    QVector<QString> maxNeighborhoodOf(const QString &gi) const; // Not const &! (a local variable in Gbk)

    QList<QString> getArchitectureList() const;
    QList<Domain> domainsOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // Not const &! (a local variable in Protein)
    QList<Domain> COGsOf(const QString &gi, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore) const; // Not const &! (a local variable in Protein)

private:
    void updateInput(const Protein &protein);
    void loadDescription(const QString &fileName, AnnotationType annotationType);

    bool okTextFile(const QString &fileName);
    bool okAFile(const QString &fileName);
    bool okPFile(const QString &fileName);
    bool okGFile(const QString &fileName);
    bool okMFile(const QString &fileName);
    bool okDFile(const QString &fileName, AnnotationType annotationType);
    bool okAppendingToTextFile(const QString &domain, const int &minscore, const int &minlength, const int &maxoverlap, const int &trustscore, const QString &fileName, const QString &textFileName);

    int trimmedCoordinate(const QString &string) const;

    //QStringList annotationOf(const QString &accession) const;

    bool gis;
    bool ids;
    bool textFileLoadingResult;
    bool membraneAnnotationLoadingResult;
    bool domainAnnotationLoadingResult;
    bool COGAnnotationLoadingResult;
    bool domainDescriptionLoadingResult;
    bool COGDescriptionLoadingResult;

    int maxRadius;

    QList<QString> architectureList;

    QMap< QString, QSet<int> > addressMap;
    QMap<QString, Protein> proteinMap;
    QMap<QString, Gbk> gbkMap;
    QMap<QString, QString> architectureMap;
    QMap<QString, QString> domainDescriptionMap;
    QMap<QString, QString> COGDescriptionMap;

    QSet<QString> allDirPaths;
    QSet<QString> inputSet;

    QString errorDescription;
    QString removedArchitecture;

    QVector<QString> inputVector;
};

#endif
