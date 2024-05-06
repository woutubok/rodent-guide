#include "rodent.h"




Rodent::Rodent(QString &suborderEng, QString &suborderRus)
{
    this->suborderEng = suborderEng;
    this->suborderRus = suborderRus;
}

Rodent::Rodent(const Rodent &other)
{
    suborderEng = other.suborderEng;
    suborderRus = other.suborderRus;
    name = other.name;
    description = other.description;
    habitat = other.habitat;
    weight = other.weight;
    maximum_age = other.maximum_age;
    image_path = other.image_path;
}

Rodent& Rodent::operator=(const Rodent& other){
    if (this != &other) {
        suborderEng = other.suborderEng;
        suborderRus = other.suborderRus;
        name = other.name;
        description = other.description;
        habitat = other.habitat;
        weight = other.weight;
        maximum_age = other.maximum_age;
        image_path = other.image_path;
    }
    return *this;
}

bool Rodent::operator ==(const Rodent &other)
{
    return name == other.name && description == other.description && habitat == other.habitat
           && weight == other.weight && maximum_age == other.maximum_age && image_path == other.image_path;
}

bool Rodent::operator !=(const Rodent &other)
{
    return !(name == other.name && description == other.description && habitat == other.habitat
             && weight == other.weight && maximum_age == other.maximum_age && image_path == other.image_path);
}

void Rodent::setSuborderEng()
{
    if(this->suborderRus == "Дикобразообразные")
        this->suborderEng = "Hystricomorpha";
    else if (this->suborderRus == "Мышеобразные")
        this->suborderEng = "Myomorpha";
    else if (this->suborderRus == "Шипохвостообразные")
        this->suborderEng = "Anomaluromorpha";
    else if (this->suborderRus == "Белкообразные")
        this->suborderEng = "Sciuromorpha";
    else
        this->suborderEng = "Castorimorpha";
}

Rodent::Rodent(Rodent &&other) noexcept: suborderEng(std::move(other.suborderEng)), suborderRus(std::move(other.suborderRus)),
    name(std::move(other.name)),
    description(std::move(other.description)),
    habitat(std::move(other.habitat)),
    weight(other.weight),
    maximum_age(other.maximum_age),
    image_path(std::move(other.image_path))
{
    other.weight = 0.0;
    other.maximum_age = 0.0;
}

Rodent& Rodent::operator=(Rodent&& other) noexcept {
    if (this != &other) {
        // Освобождаем ресурсы текущего объекта, если необходимо
        suborderEng = std::move(other.suborderEng);
        suborderRus = std::move(other.suborderRus);
        name = std::move(other.name);
        description = std::move(other.description);
        habitat = std::move(other.habitat);
        weight = other.weight;
        maximum_age = other.maximum_age;
        image_path = std::move(other.image_path);
        // Обнуляем данные в объекте other
        other.weight = 0.0;
        other.maximum_age = 0.0;
    }
    return *this;
}















