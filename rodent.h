#ifndef RODENT_H
#define RODENT_H
#include <QString>

struct Rodent
{
public:
    QString suborderEng;
    QString suborderRus;
    QString name;
    QString description;
    QString habitat;
    float weight;
    float maximum_age;
    QString image_path;

    Rodent() = default;
    Rodent(QString& suborderEng, QString& suborderRus);
    Rodent(const Rodent&);
    Rodent& operator=(const Rodent&);
    Rodent(Rodent&&) noexcept;
    Rodent& operator=(Rodent&& other) noexcept;
    bool operator ==(const Rodent& other);
    bool operator !=(const Rodent& other);
    void setSuborderEng();
};

#endif // RODENT_H
