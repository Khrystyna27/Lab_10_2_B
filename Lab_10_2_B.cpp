// Lab_10_2_B.cpp
// < Прізвище Ім’я >
// Лабораторна робота № 10.2.
// Впорядкування та бінарний пошук в масиві структур
// Варіант 30, рівень B
// Сформувати динамічний масив структур з об’єднаннями про студентів (прізвище, курс,
// спеціальність, оцінки з фізики, математики, третя оцінка – з програмування (KN), чисельних методів (IN), педагогіки (ME, FI, TN)).
// Програма має меню та контроль помилок введення:
//   1. фізичне впорядкування масиву: в першу чергу – за значенням середнього балу,
//      в другу чергу (для однакових середніх балів) – за номером курсу,
//      в останню чергу (для однакових середніх балів і курсів) – за прізвищем
//      за спаданням – в зворотному до алфавітного порядку;
//   2. індексне впорядкування: в першу чергу – за середнім балом, в другу чергу –
//      за номером курсу, в останню чергу – за прізвищем (в алфавітному порядку);
//   3. бінарний пошук (у фізично впорядкованому масиві): чи навчається студент
//      із вказаним прізвищем на вказаному курсі та з вказаним середнім балом.
 
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <limits>
using namespace std;
 
enum Specialnist { KN, IN, ME, FI, TN };
const string specStr[] = { "Комп’ютерні науки", "Інформатика",
                          "Математика та економіка", "Фізика та інформатика",
                          "Трудове навчання" };
 
struct Student
{
   string       prizv;
   unsigned     kurs;
   Specialnist  spec;
   int          fizyka;
   int          matematyka;
   union                       // третя оцінка залежить від спеціальності
   {
       int      prohramuvannya;    // KN
       int      chyselni_metody;   // IN
       int      pedahohika;        // ME, FI, TN
   };
};
 
const double EPS = 0.005;           // точність порівняння середніх балів
 
int    InputInt(const string &prompt, const int lo, const int hi);
string InputSurname(const string &prompt);
void   Create(Student *p, const int N);
void   Print(Student *p, const int N);
int    ThirdMark(const Student &s);
double Avg(const Student &s);
bool   Greater(const Student &a, const Student &b);
void   Sort(Student *p, const int N);
int    BinSearch(Student *p, const int N, const string prizv,
                const unsigned kurs, const double avg);
int   *IndexSort(Student *p, const int N);
void   PrintIndexSorted(Student *p, int *I, const int N);
 
#ifndef UNIT_TEST           // під час unit-тестування main не компілюється
int main()
{
   int N;
   N = InputInt("Введіть кількість студентів N: ", 1, 1000);
   Student *p = new Student[N];
   int menuItem;
   int found;
   string prizv;
   unsigned kurs;
   double avg;
   do {
       cout << endl << endl << endl;
       cout << "Виберіть дію:" << endl << endl;
       cout << " [1] - введення даних з клавіатури" << endl;
       cout << " [2] - вивід даних на екран" << endl;
       cout << " [3] - фізичне впорядкування даних" << endl;
       cout << " [4] - бінарний пошук студента за прізвищем, курсом та середнім балом" << endl;
       cout << "       (спочатку виконайте фізичне впорядкування!)" << endl;
       cout << " [5] - індексне впорядкування та вивід даних" << endl << endl;
       cout << " [0] - вихід та завершення роботи програми" << endl << endl;
       menuItem = InputInt("Введіть значення: ", 0, 5);
       cout << endl << endl << endl;
       switch (menuItem)
       {
       case 1:
           Create(p, N);
           break;
       case 2:
           Print(p, N);
           break;
       case 3:
           Sort(p, N);
           break;
       case 4:
           cout << "Введіть ключі пошуку:" << endl;
           prizv = InputSurname("    прізвище: ");
           kurs = InputInt("    курс (1-6): ", 1, 6);
           cout << "    середній бал (напр. 4.33): ";
           while (!(cin >> avg) || avg < 2 || avg > 5)
           {
               cin.clear();
               cin.ignore(numeric_limits<streamsize>::max(), '\n');
               cout << "Помилка! Слід ввести число від 2 до 5: ";
           }
           cin.ignore(numeric_limits<streamsize>::max(), '\n');
           cout << endl;
           if ((found = BinSearch(p, N, prizv, kurs, avg)) != -1)
               cout << "Знайдено студента в позиції " << found + 1 << endl;
           else
               cout << "Шуканого студента не знайдено" << endl;
           break;
       case 5:
           {
               int *I = IndexSort(p, N);
               PrintIndexSorted(p, I, N);
               delete[] I;
           }
           break;
       case 0:
           break;
       default:
           cout << "Ви ввели помилкове значення! "
                   "Слід ввести число - номер вибраного пункту меню" << endl;
       }
   } while (menuItem != 0);
   delete[] p;
   return 0;
}
#endif
 
// Вводить ціле число з контролем помилок: повторює запит, поки не буде
// введено число з діапазону [lo, hi]
int InputInt(const string &prompt, const int lo, const int hi)
{
   int value;
   while (true)
   {
       cout << prompt;
       if (cin >> value && value >= lo && value <= hi)
       {
           cin.ignore(numeric_limits<streamsize>::max(), '\n');  // видаляємо залишок рядка
           return value;
       }
       if (cin.eof())
           exit(1);         // введення завершено – продовжувати неможливо
       cin.clear();                        // скидаємо стан помилки потоку
       cin.ignore(numeric_limits<streamsize>::max(), '\n');
       cout << "Помилка! Слід ввести ціле число від " << lo << " до " << hi << "." << endl;
   }
}
 
// Вводить прізвище з контролем помилок: непорожній рядок без цифр
string InputSurname(const string &prompt)
{
   string s;
   while (true)
   {
       cout << prompt;
       if (!getline(cin, s))
           exit(1);
       bool ok = !s.empty();
       for (char c : s)
           if (c >= '0' && c <= '9')
               ok = false;
       if (ok)
           return s;
       cout << "Помилка! Прізвище не може бути порожнім або містити цифри." << endl;
   }
}
 
// Повертає рядок s, доповнений пробілами до ширини width (кількість символів UTF-8,
// а не байтів – щоб таблиця з кирилицею була вирівняна на macOS/Linux)
string Cell(const string &s, const int width, const bool leftAlign = true)
{
   int len = 0;
   for (unsigned char c : s)
       if ((c & 0xC0) != 0x80)        // рахуємо лише перші байти символів
           len++;
   string pad(len < width ? width - len : 0, ' ');
   return leftAlign ? s + pad : pad + s;
}
 
void Create(Student *p, const int N)
{
   for (int i = 0; i < N; i++)
   {
       cout << "Студент № " << i + 1 << ":" << endl;
       p[i].prizv = InputSurname("    прізвище: ");
       p[i].kurs = InputInt("    курс (1-6): ", 1, 6);
       p[i].spec = (Specialnist)InputInt("    спеціальність (0 - Комп’ютерні науки, 1 - Інформатика,\n"
                                         "                   2 - Математика та економіка, 3 - Фізика та інформатика,\n"
                                         "                   4 - Трудове навчання): ", 0, 4);
       p[i].fizyka = InputInt("    оцінка з фізики (2-5): ", 2, 5);
       p[i].matematyka = InputInt("    оцінка з математики (2-5): ", 2, 5);
       switch (p[i].spec)
       {
       case KN:
           p[i].prohramuvannya = InputInt("    оцінка з програмування (2-5): ", 2, 5);
           break;
       case IN:
           p[i].chyselni_metody = InputInt("    оцінка з чисельних методів (2-5): ", 2, 5);
           break;
       case ME:
       case FI:
       case TN:
           p[i].pedahohika = InputInt("    оцінка з педагогіки (2-5): ", 2, 5);
           break;
       }
       cout << endl;
   }
}
 
void Print(Student *p, const int N)
{
   cout << "Таблиця студентів" << endl;
   cout << string(107, '=') << endl;
   cout << "| " << Cell("№", 3) << " | " << Cell("Прізвище", 12) << " | " << Cell("Курс", 4)
        << " | " << Cell("Спеціальність", 23) << " | " << Cell("Фізика", 6)
        << " | " << Cell("Матем.", 6) << " | " << Cell("Програм.", 8)
        << " | " << Cell("Числ.мет.", 9) << " | " << Cell("Педагог.", 8) << " |" << endl;
   cout << string(107, '-') << endl;
   for (int i = 0; i < N; i++)
   {
       cout << "| " << setw(3) << right << i + 1
            << " | " << Cell(p[i].prizv, 12)
            << " | " << setw(4) << right << p[i].kurs
            << " | " << Cell(specStr[p[i].spec], 23)
            << " | " << setw(6) << right << p[i].fizyka
            << " | " << setw(6) << right << p[i].matematyka;
       switch (p[i].spec)
       {
       case KN:
           cout << " | " << setw(8) << right << p[i].prohramuvannya
                << " | " << setw(9) << " " << " | " << setw(8) << " " << " |" << endl;
           break;
       case IN:
           cout << " | " << setw(8) << " "
                << " | " << setw(9) << right << p[i].chyselni_metody
                << " | " << setw(8) << " " << " |" << endl;
           break;
       case ME:
       case FI:
       case TN:
           cout << " | " << setw(8) << " " << " | " << setw(9) << " "
                << " | " << setw(8) << right << p[i].pedahohika << " |" << endl;
           break;
       }
   }
   cout << string(107, '=') << endl;
   cout << endl;
}
 
// Повертає третю оцінку студента – залежно від його спеціальності
int ThirdMark(const Student &s)
{
   switch (s.spec)
   {
   case KN: return s.prohramuvannya;
   case IN: return s.chyselni_metody;
   default: return s.pedahohika;
   }
}
 
// Обчислює середній бал студента
double Avg(const Student &s)
{
   return (s.fizyka + s.matematyka + ThirdMark(s)) / 3.0;
}
 
// Повертає true, якщо студент a має стояти ПІСЛЯ студента b у фізично впорядкованому
// масиві: за середнім балом (зростання), за курсом (зростання), за прізвищем (спадання)
bool Greater(const Student &a, const Student &b)
{
   if (Avg(a) != Avg(b))
       return Avg(a) > Avg(b);
   if (a.kurs != b.kurs)
       return a.kurs > b.kurs;
   return a.prizv < b.prizv;
}
 
// Фізичне впорядкування масиву (метод "бульбашки")
void Sort(Student *p, const int N)
{
   Student tmp;
   for (int i0 = 0; i0 < N - 1; i0++)
       for (int i1 = 0; i1 < N - i0 - 1; i1++)
           if (Greater(p[i1], p[i1 + 1]))
           {
               tmp = p[i1];
               p[i1] = p[i1 + 1];
               p[i1 + 1] = tmp;
           }
}
 
// Бінарний пошук у фізично впорядкованому масиві за ключами (прізвище, курс, середній бал);
// повертає індекс знайденого елемента або -1, якщо шуканий елемент відсутній
int BinSearch(Student *p, const int N, const string prizv,
             const unsigned kurs, const double avg)
{
   int L = 0, R = N - 1, m;
   while (L <= R)
   {
       m = (L + R) / 2;
       double a = Avg(p[m]);
       bool sameAvg = fabs(a - avg) < EPS;
       if (sameAvg && p[m].kurs == kurs && p[m].prizv == prizv)
           return m;
 // елемент p[m] стоїть ПЕРЕД шуканим ключем (порядок: середній бал, курс, прізвище за спаданням)
       if ((!sameAvg && a < avg)
           ||
           (sameAvg && p[m].kurs < kurs)
           ||
           (sameAvg && p[m].kurs == kurs && p[m].prizv > prizv))
       {
           L = m + 1;
       }
       else
       {
           R = m - 1;
       }
   }
   return -1;
}
 
// Будує індексний масив для логічного впорядкування: за середнім балом (зростання),
// за курсом (зростання), за прізвищем (зростання – в алфавітному порядку)
int *IndexSort(Student *p, const int N)
{ // використовуємо метод вставки для формування індексного масиву
   int *I = new int[N];    // створили індексний масив
   for (int i = 0; i < N; i++)
       I[i] = i;           // заповнили його початковими даними
   int i, j, value;        // починаємо сортувати масив індексів
   for (i = 1; i < N; i++)
   {
       value = I[i];
       for (j = i - 1;
            j >= 0 && ((Avg(p[I[j]]) > Avg(p[value])) ||
                       (Avg(p[I[j]]) == Avg(p[value]) && p[I[j]].kurs > p[value].kurs) ||
                       (Avg(p[I[j]]) == Avg(p[value]) && p[I[j]].kurs == p[value].kurs &&
                        p[I[j]].prizv > p[value].prizv));
            j--)
       {
           I[j + 1] = I[j];
       }
       I[j + 1] = value;
   }
   return I;
}
 
// Виводить масив у порядку, визначеному індексним масивом I
void PrintIndexSorted(Student *p, int *I, const int N)
{
   Student *q = new Student[N];
   for (int i = 0; i < N; i++)
       q[i] = p[I[i]];  // доступ до елементів за допомогою індексного масиву
   Print(q, N);
   delete[] q;
}
