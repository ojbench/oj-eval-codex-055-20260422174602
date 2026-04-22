// Implementation for Problem 055 - ACM EXPRESS (ACMOJ 1421)
// Single header to be included by OJ main.cpp

#include <iostream>
#include <algorithm>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
};

inline istream& operator>>(istream& in, date& d) {
    return (in >> d.year >> d.month >> d.day);
}

inline bool operator<(const date& a, const date& b) {
    if (a.year != b.year) return a.year < b.year;
    if (a.month != b.month) return a.month < b.month;
    return a.day < b.day;
}

class mail : public object {
protected:
    string postmark;
    date send_date{};
    date arrive_date{};

public:
    mail() = default;
    mail(string _contain_, string _postmark_, date send_d, date arrive_d)
        : object(_contain_), postmark(_postmark_), send_date(send_d), arrive_date(arrive_d) {}
    virtual ~mail() = default;

    string send_status(int, int, int) override { return "not send"; }
    string type() override { return "no type"; }
    void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << '\n';
    }
    void copy(object* o) override {
        contain = reinterpret_cast<mail*>(o)->contain;
        postmark = reinterpret_cast<mail*>(o)->postmark;
        send_date = reinterpret_cast<mail*>(o)->send_date;
        arrive_date = reinterpret_cast<mail*>(o)->arrive_date;
    }
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date{};
    date land_date{};
public:
    air_mail() = default;
    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
             date take_off, date land, string _airline)
        : mail(_contain_, _postmark_, send_d, arrive_d),
          airlines(_airline), take_off_date(take_off), land_date(land) {}
    ~air_mail() override = default;

    string send_status(int y, int m, int d) override {
        date ask(y, m, d);
        if (ask < send_date) return "mail not send";
        else if (ask < take_off_date) return "wait in airport";
        else if (ask < land_date) return "in flight";
        else if (ask < arrive_date) return "already land";
        else return "already arrive";
    }
    string type() override { return "air"; }
    void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << '\n';
    }
    void copy(object* o) override {
        auto* a = reinterpret_cast<air_mail*>(o);
        contain = a->contain;
        postmark = a->postmark;
        send_date = a->send_date;
        arrive_date = a->arrive_date;
        take_off_date = a->take_off_date;
        land_date = a->land_date;
        airlines = a->airlines;
    }
};

class train_mail : public mail {
protected:
    string* station_name = nullptr;
    date* station_time = nullptr;
    int len = 0;
public:
    train_mail() = default;
    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d,
               string* sname, date* stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d), len(station_num) {
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        }
    }
    ~train_mail() override {
        delete[] station_name;
        delete[] station_time;
    }

    string send_status(int y, int m, int d) override {
        date q(y, m, d);
        if (q < send_date) return "mail not send";
        if (len == 0) {
            if (q < arrive_date) return "on the way";
            return "already arrive";
        }
        if (q < station_time[0]) return "wait in station";
        for (int i = 0; i + 1 < len; ++i) {
            if (q < station_time[i + 1]) {
                return string("between ") + station_name[i] + " and " + station_name[i + 1];
            }
        }
        if (q < arrive_date) return string("at ") + station_name[len - 1];
        return "already arrive";
    }
    string type() override { return "train"; }
    void print() override {
        mail::print();
        cout << "[train] station_num: " << len << '\n';
    }
    void copy(object* o) override {
        auto* t = reinterpret_cast<train_mail*>(o);
        contain = t->contain;
        postmark = t->postmark;
        send_date = t->send_date;
        arrive_date = t->arrive_date;
        delete[] station_name;
        delete[] station_time;
        len = t->len;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = t->station_name[i];
                station_time[i] = t->station_time[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }
};

class car_mail : public mail {
protected:
    int total_mile = 0;
    string driver;
public:
    car_mail() = default;
    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d, int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d), total_mile(mile), driver(_driver) {}
    ~car_mail() override = default;

    string send_status(int y, int m, int d) override {
        date ask(y, m, d);
        if (ask < send_date) return "mail not send";
        if (!(ask < arrive_date)) return "already arrive"; // ask >= arrive_date
        auto to_days = [](const date& dt) { return dt.year * 360 + dt.month * 30 + dt.day; };
        double elapsed = static_cast<double>(to_days(ask) - to_days(send_date));
        double total = static_cast<double>(to_days(arrive_date) - to_days(send_date));
        double current_mile = total > 0.0 ? (elapsed / total) * static_cast<double>(total_mile) : 0.0;
        return to_string(current_mile);
    }
    string type() override { return "car"; }
    void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << '\n';
    }
    void copy(object* o) override {
        auto* c = reinterpret_cast<car_mail*>(o);
        contain = c->contain;
        postmark = c->postmark;
        send_date = c->send_date;
        arrive_date = c->arrive_date;
        total_mile = c->total_mile;
        driver = c->driver;
    }
};

inline void obj_swap(object*& lhs, object*& rhs) {
    object* tmp = lhs; lhs = rhs; rhs = tmp;
}
