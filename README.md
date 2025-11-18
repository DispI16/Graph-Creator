**Якщо ви малюєте графи для лаби, перед початком захисту кажіть, що програму написав студент СШІ Співак Димитрій**

# Graph builder / Редактор графів

Це програма для створення графів, написана за допомогою бібліотеки SFML та ImGui.\
Вона використовує CMake для компіляції. Шаблон для проєктів на SFML/ImGui я взяв за посиланням https://github.com/morgunovmi/imgui-docking-sfml-starter

Application for building graphs created with SFML and ImGui.\
Uses Cmake for compilation. Template for SFML/ImGui projects provided by https://github.com/morgunovmi/imgui-docking-sfml-starter

**ВАЖЛИВО:** потрібно змінити `#define SAVE_PATH` та `#define FONT_PATH` на лініях 14 та 15 у `main.cpp` на відповідні шляхи на вашому комп'ютері. \
**IMPORTANT:** change `#define SAVE_PATH` and `#define FONT_PATH` on line 14 and 15 in `main.cpp` to respective locations on your PC

## Showcase / Демонстрація
![Screenshot 1](./screenshots/Graph_Subgraph_Union.png?raw=true)
![Screenshot 2](/screenshots/Graph_3.png?raw=true)
![Screenshot 3](./screenshots/Graph_Product.png?raw=true)
![Screenshot 4](/screenshots/Min_Dist_V_1.png?raw=true)

## How To Use / Як використовувати
+ Grid: when turned on, placed objects will snap to grid / Коли ввімкнено, відображатиметься сітка. Під час руху та створення об'єкти прив'язуватимуться до сітки.
+ Rect: Only objects inside the rectangle will be saved to image. Use numbers to control topleft and bottomright position of the rectangle / Показує прямокутник, об'єкти всередині якого будуть збережені до зображення. Змінюйте цифрові значення, щоб змінити координати верхнього лівого та правого нижгього кутів прямокутника.
+ Lock edges to vertices: if this is on, the edges' third point will update together with the updated vertices / Якщо ця опція ввімкнена, то позиція третьої точки ребра буде змінюватися у відповідності зі зміною позиції вершин.

+ Create vertices: click to create vertex / натисніть, щоб створити вершину
+ Create edges: select 2 vertices to create an edge / виберіть 2 вершини, щоб стоврити ребро.
+ Create text: Input text into respective field and click anywhere to add it / введіть текст у відповідне поле та натисніть, щоб створити текстовий об'єкт
+ Delete vertices/edges/text: click on an object to delete it / натисніть на об'єкт, щоб його видалити
+ Modify vertices/edges/text: click on an object and modify it / натисніть на об'єкт, щоб його модифікувати. Можна змінити позицію вибраного об'єкта, натиснувши будь-куди.
+ Oriented Edges: Doesn't work yet / Орієнтовані ребра поки не працюють. Також не працюють петлі.
+ Save graph as image: saves graph to `filename`.png in `SAVE_PATH` / збергіає граф як зображення `filename`.png в папці, яку ви вказали в `SAVE_PATH`
+ Save graph as file: saves graph to `filename`.txt in `SAVE_PATH` / збергіає граф у файлі `filename`.txt в `SAVE_PATH` для подальшого завантаження та редагування
+ Load graph from file: loads graph from `filename`.txt in `SAVE_PATH` / Завантажує граф з файла `filename`.txt у `SAVE_PATH`
+ Repaint all vertices/edges: Repaints all vertices/edges to selected color / Змінює колір усіх ребер або вершин у графі на обраний
+ Place in a circle: places all vertices on the screen in a circle inside of the rendering rect / Розміщує всі вершини графа в коло всередині прямокутника для рендерингу.

# How to build / Як компілювати
Для компіляції потрібно встановити CMake. Компілювати можна через VSCode або через термінал. \
You need to install CMake before compilation. After installation build it using CMake (optionally inside VSCode).

# If you find a bug / Якщо ви знайдете баг
Можете відкрити issue на цій сторінці. Можливо я його виправлю, може - ні.\
You can open an issue. It may or may not be fixed in future.

# If you want to make changes to code / Якщо ви хочете змінити код
Стовріть pull request. Можливо я його змерджу, можливо - ні.
Create a pull request. It may or may not be added in future.

# If you want to copy code / Якщо ви хочете скопіювати код
Копіюйте на здоровля \
You can do anything with the code