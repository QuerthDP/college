/*
 * =====================================================
 * Szósty projekt zaliczeniowy z
 * przedmiotu Kurs Programowania w C++ (1000-213bCPP).
 *
 * Plik nagłówkowy modułu 'college'.
 *
 * Plik zawiera implementacje klas wykorzystywanych
 * do zarządzania uczelnią.
 *
 * Autorzy:
 * Dawid Pawlik      <dp450244@students.mimuw.edu.pl>
 * Michał Racinowski <mr450260@students.mimuw.edu.pl>
 * =====================================================
 */


/* Początek deklaracji elementów modułu college.h */

#ifndef _COLLEGE_H
#define _COLLEGE_H


/* Pliki nagłówkowe wykorzystywane do implementacji modułu */

#include <memory>
#include <set>
#include <string>
#include <compare>
#include <type_traits>
#include <regex>
#include <vector>
#include <stdexcept>


/* Deklaracje implementowanych klas */

class Course;
class Person;
class Student;
class Teacher;
class PhDStudent;
class College;


/* Przestrzeń nazw 'utils' zawierająca klasy i funkcje pomocnicze */

namespace utils {

    const std::string ERROR_NONEXISTANT_PERSON = "Non-existing person.";
    const std::string ERROR_NONEXISTANT_COURSE = "Non-existing course.";
    const std::string ERROR_INACTIVE_COURSE =
        "Incorrect operation on an inactive course.";
    const std::string ERROR_INACTIVE_STUDENT =
        "Incorrect operation for an inactive student.";

    /*
     * Koncept do weryfikacji czy obiekt typu 'T' jest
     * klasą 'Person' lub jej klasą potomną.
     */
    template <typename T>
    concept PersonBased = std::is_base_of_v<Person, T>;

    /*
     * Koncept do weryfikacji czy obiekt typu 'T' dodawany na uczelnie
     * jest klasą potomną klasy 'Person'.
     *
     * Założenie: nie dodajemy na uczelnie niewyspecyfikowanych
     * osób, zaś wszystkie klasy potomne klasy 'Person' są w stanie
     * zaistnieć na uczelni, np. 'Security', 'Dean', 'Janitor', etc.
     */
    template <typename T>
    concept CollegePerson = PersonBased<T> && !std::is_same_v<Person, T>;

    /*
     * Koncept do weryfikacji czy obiekt typu 'T' może znajdować się
     * na uczelni oraz może uczestniczyć w jakimkolwiek przedmiocie.
     */
    template <typename T>
    concept CourseRelatedPerson = CollegePerson<T> &&
                                  requires(T t) { t.get_courses(); };

    /*
     * Funkcja porównująca wartości wskazywane przez 'shared_ptr<T>'.
     *
     * Założenie: typ 'T' ma zdefiniowany operator '<'.
     */
    template <typename T>
    bool compare(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
                 noexcept {
        return *a < *b;
    }

    /*
     * Alias na uporządkowany zbiór typu 'shared_ptr<T>'.
     */
    template <typename T>
    using set_ptr = std::set<std::shared_ptr<T>, decltype(&utils::compare<T>)>;

    /*
     * Alias na wektor typu 'shared_ptr<T>'.
     */
    template <typename T>
    using vec_ptr = std::vector<std::shared_ptr<T>>;


    /*
     * Klasa pomocnicza 'Pattern' wykorzystywana do obsługi wzorca
     * wykorzystując 'regex' i 'regex_match'.
     */
    class Pattern {
        static constexpr std::string REGEX_BEGIN = "^";
        static constexpr std::string REGEX_END = "$";
        static constexpr char PATTERN_ANY = '?';
        static constexpr std::string REGEX_ANY= ".";
        static constexpr char PATTERN_WILDCARD = '*';
        static constexpr std::string REGEX_WILDCARD = ".*";

        std::regex regex;

        /*
         * Funkcja tworzy 'regex' na podstawie przekazanego 'pattern'.
         */
        const std::regex pattern_to_regex(const std::string &pattern) {
            std::string regex = REGEX_BEGIN;

            for (char i: pattern) {
                switch (i) {
                    case PATTERN_WILDCARD:
                        regex += REGEX_WILDCARD;
                        break;

                    case PATTERN_ANY:
                        regex += REGEX_ANY;
                        break;

                    default:
                        regex += i;
                }
            }
            regex += REGEX_END;

            return std::regex(regex);
        }

    public:
        /*
         * Konstruktor klasy 'Pattern' konwertujący 'pattern' na 'regex'.
         */
        Pattern(const std::string &pattern) :
            regex(pattern_to_regex(pattern)) {

        }

        /*
         * Funkcja sprawdza czy 'text' pasuje do przechowywanego wzorca 'regex'.
         */
        bool match(const std::string &text) const {
            return std::regex_match(text, regex);
        }
    };

} // Koniec przestrzeni nazw 'utils'.


/* Implementacja klas dostępnych w module */

/*
 * Klasa 'Course' wykorzystywana do przechowywania informacji
 * o nazwie i statusie aktywności przedmiotu.
 */
class Course {
    std::string name;
    bool active;

public:
    /*
     * Konstruktor klasy 'Course' przyjmujący nazwę
     * i opcjonalnie status aktywności domyślnie ustawiony na 'true'.
     */
    Course(const std::string& name, bool active = true) :
        name(name),
        active(active) {

    }

    /* Konstruktor bezparametrowy został domyślnie usunięty
       wraz ze zdefiniowaniem konstruktora z parametrami */

    /*
     * Funkcja przekazuje niemodyfikowalny dostęp do nazwy przedmiotu
     */
    const std::string& get_name() const noexcept {
        return name;
    }

    /*
     * Funkcja informuje czy przedmiot jest aktywny.
     */
    bool is_active() const noexcept {
        return active;
    }

    /*
     * Funkcja ustawia status aktywności przedmiotu na wartość 'active'.
     */
    void set_active(bool active) noexcept {
        this->active = active;
    }

    /*
     * Operator porównania trójkierunkowego określony na nazwie przedmiotu.
     */
    std::strong_ordering operator<=>(const Course& other) const noexcept {
        return name <=> other.name;
    }

};


/*
 * Klasa 'Person' wykorzystywana do przechowywania
 * informacji o danych osobowych danej osoby.
 */
class Person {
    std::string name;
    std::string surname;

public:
    /*
     * Konstruktor klasy 'Person' przyjmujący imię i nazwisko osoby.
     */
    Person(const std::string& name, const std::string& surname) :
        name(name),
        surname(surname) {

    }

    /* Konstruktor bezparametrowy został domyślnie usunięty
       wraz ze zdefiniowaniem konstruktora z parametrami */

    /*
     * Destruktor domyślny oznaczony jako 'virtual',
     * aby umożliwić odpowiednie dziedziczenie.
     */
    virtual ~Person() noexcept = default;

    /*
     * Funkcja przekazuje niemodyfikowalny dostęp do imienia osoby.
     */
    const std::string& get_name() const noexcept {
        return name;
    }

    /*
     * Funkcja przekazuje niemodyfikowalny dostęp do nazwiska osoby.
     */
    const std::string& get_surname() const noexcept {
        return surname;
    }

    /*
    * Operator porównania trójkierunkowego określony na imieniu i nazwisku.
    */
    std::strong_ordering operator<=>(const Person& other) const noexcept {
        if (surname < other.surname)
            return std::strong_ordering::less;
        if (surname > other.surname)
            return std::strong_ordering::greater;
        if (name < other.name)
            return std::strong_ordering::less;
        if (name > other.name)
            return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

};


/*
 * Klasa 'Student' dziedzicząca po klasie 'Person' wykorzystywana
 * do reprezentacji studenta, posiadającego przedmioty na które uczęszcza
 * oraz status swojej aktywności na uczelni.
 */
class Student : virtual public Person {
    utils::set_ptr<Course> courses;
    bool active;

public:
    /*
     * Konstruktor klasy 'Student' przyjmujący imię i nazwisko
     * studenta oraz jego status aktywności domyślnie ustawiony na 'true'.
     */
    Student(const std::string& name, const std::string& surname,
            bool active = true) :
        Person(name, surname),
        courses{&utils::compare},
        active(active) {

    }

    /* Konstruktor bezparametrowy został domyślnie usunięty
       wraz ze zdefiniowaniem konstruktora z parametrami */

    /*
     * Funkcja informuje, czy student jest aktywny.
     */
    bool is_active() const noexcept {
        return active;
    }

    /*
     * Funkcja przekazuje niemodyfikowalny zbiór
     * przedmiotów uczęszczanych przez studenta.
     */
    const utils::set_ptr<Course>& get_courses() const noexcept {
        return courses;
    }

    /*
     * Funkcja ustawia status aktywności studenta na wartość 'active'.
     */
    void set_active(bool active) noexcept {
        this->active = active;
    }

    /*
     * Funkcja dodaje nowy przedmiot 'course' uczęszczany przez studenta.
     */
    bool assign_course(const std::shared_ptr<Course> &course) {
        if (!active)
            throw std::runtime_error(utils::ERROR_INACTIVE_STUDENT);

        return courses.insert(course).second;
    }

};


/*
 * Klasa 'Teacher' dziedzicząca po klasie 'Person' wykorzystywana
 * do reprezentacji prowadzącego, posiadającego przedmioty które prowadzi.
 */
class Teacher : virtual public Person {
    utils::set_ptr<Course> courses;

public:
    /*
     * Konstruktor klasy 'Teacher' przyjmujący imię i nazwisko prowadzącego.
     */
    Teacher(const std::string& name, const std::string& surname) :
        Person(name, surname),
        courses{&utils::compare} {

    }

    /*
     * Konstruktor klasy 'Teacher' przyjmujący imię, nazwisko
     * i nieużywany parametr typu 'bool' w celu uogólnienia,
     * podczas dodawania metodą 'College::add_person'.
     */
    Teacher(const std::string& name, const std::string& surname, bool) :
        Teacher(name, surname) {

    }

    /* Konstruktor bezparametrowy został domyślnie usunięty
       wraz ze zdefiniowaniem konstruktora z parametrami */

    /*
     * Funkcja przekazuje niemodyfikowalny zbiór
     * przedmiotów prowadzonych przez prowadzącego.
     */
    const utils::set_ptr<Course>& get_courses() const noexcept {
        return courses;
    }

    /*
     * Funkcja dodaje nowy przedmiot 'course' prowadzony przez prowadzącego.
     */
    bool assign_course(const std::shared_ptr<Course> &course) {
        return courses.insert(course).second;
    }

};


/*
 * Klasa 'PhDStudent' dziedzicząca wielokrotnie po klasie 'Student' oraz
 * po klasie 'Teacher' wykorzystywana do reprezentacji doktoranta.
 */
class PhDStudent : public Student, public Teacher {
public:
    /*
     * Konstruktor klasy 'PhDStudent' przyjmujący imię i nazwisko doktoranta
     * oraz jego status aktywności domyślnie ustawiony na 'true'.
     */
    PhDStudent(const std::string& name, const std::string& surname,
               bool active = true) :
        Person(name, surname),
        Student(name, surname, active),
        Teacher(name, surname) {

    }

    /* Konstruktor bezparametrowy został domyślnie usunięty
       wraz ze zdefiniowaniem konstruktora z parametrami */

};


/*
 * Klasa 'College' wykorzystywana do reprezentacji uczelni i zarządzania nią.
 */
class College {
    utils::set_ptr<Course> courses{&utils::compare};
    utils::set_ptr<Person> people{&utils::compare};

public:
    /*
     * Funkcja dodaje nowy przedmiot o nazwie 'name' i statusie
     * aktywności 'active' domyślnie ustawionym na 'true'
     * do zbioru przedmiotów wykładanych na uczelni.
     */
    bool add_course(const std::string &name, bool active = true) {
        return courses.insert(std::make_shared<Course>(name, active)).second;
    }

    /*
     * Funkcja przekazuje posortowany alfabetycznie według nazw przedmiotów
     * wektor przedmiotów, o nazwach pasujących do wzorca 'pattern'.
     */
    const utils::vec_ptr<Course> find_courses(const std::string &pattern)
                                              const {
        utils::Pattern course_pattern(pattern);
        utils::vec_ptr<Course> result;

        for (auto const& course : courses) {
            if (course_pattern.match(course->get_name())) {
                result.push_back(course);
            }
        }

        return result;
    }

    /*
     * Funkcja zmienia status aktywności przedmiotu 'course' na 'active'.
     */
    bool change_course_activeness(const std::shared_ptr<Course> &course,
                                  bool active) noexcept {
        auto found = courses.find(course);

        if (found != courses.end() && course == *found) {
            course->set_active(active);
            return true;
        }

        return false;
    }

    /*
     * Funkcja usuwa przedmiot 'course' ze zbioru przedmiotów wykładanych
     * na uczelni oraz zmienia jego aktywność na 'false'.
     */
    bool remove_course(const std::shared_ptr<Course> &course) noexcept {
        auto found = courses.find(course);

        if (found == courses.end())
            return false;

        if (course != *found)
            return false;

        if (!courses.erase(course))
            return false;

        course->set_active(false);

        return true;
    }

    /*
     * Funkcja dodaje do zbioru osób na uczelni osobę spełniającą koncept
     * 'utils::College_Person' o imieniu 'name', nazwisku 'surname' oraz
     * statusie aktywności 'active' domyślnie ustawionym na 'true'.
     */
    template <typename T>
    requires utils::CollegePerson<T>
    bool add_person(const std::string& name, const std::string& surname,
                    bool active = true) {
        return people.insert(std::make_shared<T>(name, surname, active)).second;
    }

    /*
     * Funkcja zmienia status aktywności studenta 'student' na 'active'.
     */
    bool change_student_activeness(const std::shared_ptr<Student>& student,
                                   bool active) noexcept {
        auto found = people.find(student);

        if (found != people.end() && student == *found) {
            student->set_active(active);
            return true;
        }

        return false;
    }

    /*
     * Funkcja przekazuje posortowany alfabetycznie według nazwisk i imion
     * wektor osób typu 'T', o imionach i nazwiskach
     * pasujących do wzorców 'name' i 'surname'.
     */
    template <typename T>
    requires utils::PersonBased<T>
    const utils::vec_ptr<T> find(const std::string& name,
                                 const std::string& surname) const {
        utils::Pattern name_pattern(name);
        utils::Pattern surname_pattern(surname);
        utils::vec_ptr<T> result;

        for (auto const& person : people) {
            std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(person);

            if (cast &&
                name_pattern.match(person->get_name()) &&
                surname_pattern.match(person->get_surname())) {
                result.push_back(cast);
            }
        }

        return result;
    }

    /*
     * Funkcja przekazuje posortowany alfabetycznie według nazwisk i imion
     * wektor osób typu 'T' uczęszczających na przedmiot 'course'.
     */
    template <typename T>
    requires utils::CourseRelatedPerson<T>
    const utils::vec_ptr<T> find(const std::shared_ptr<Course> &course) const {
        utils::vec_ptr<T> result;

        for (auto const& person : people) {
            std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(person);

            if (cast) {
                auto found = cast->get_courses().find(course);
                if (found != cast->get_courses().end() && course == *found) {
                    result.push_back(cast);
                }
            }
        }

        return result;
    }

    /*
     * Funkcja dodaje osobie 'person' przedmiot 'course' do zbioru
     * przedmiotów, w których osoba ma swój udział.
     */
    template <typename T>
    requires utils::CourseRelatedPerson<T>
    bool assign_course(const std::shared_ptr<T> &person,
                       const std::shared_ptr<Course> &course) {
        auto found_person = people.find(person);
        if (found_person == people.end() || person != *found_person)
            throw std::runtime_error(utils::ERROR_NONEXISTANT_PERSON);

        auto found_course = courses.find(course);
        if (found_course == courses.end() || course != *found_course)
            throw std::runtime_error(utils::ERROR_NONEXISTANT_COURSE);

        if (!course->is_active())
            throw std::runtime_error(utils::ERROR_INACTIVE_COURSE);

        return person->assign_course(course);
    }

};


#endif // _COLLEGE_H
