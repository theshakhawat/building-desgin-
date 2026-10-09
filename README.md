# 🏢 4-Floor 3D Building Design with Full Interior Furniture (KUET CSE)

একটি পূর্ণাঙ্গ ৪-তলা বিশিষ্ট ৩ডি বিল্ডিং ডিজাইন (Ground Floor + 1st + 2nd + 3rd + 4th Floor + Rooftop), যা খুলনা প্রকৌশল ও প্রযুক্তি বিশ্ববিদ্যালয় (KUET) CSE ডিপার্টমেন্টের **Computer Graphics Laboratory (Lab-01 & Lab-02)**-এর সিলেবাস এবং থিওরি হুবহু অনুসরণ করে তৈরি করা হয়েছে।

এই প্রজেক্টটিতে আর্কিটেকচারাল ফ্যাসাডে **জানালা এবং ব্যালকনি সম্পূর্ণ আলাদা আলাদা উইংয়ে (Window on Left Wing & Balcony on Right Wing)** সাজানো হয়েছে। ব্যালকনির ভেতর কোনো অপ্রয়োজনীয় জানালা নেই; ব্যালকনিতে বের হওয়ার জন্য সুসজ্জিত গ্লাস ডোর রয়েছে এবং অপর পাশে প্রশস্ত আধুনিক কাঁচের জানালা স্থাপন করা হয়েছে।

---

## 📑 সূচিপত্র (Table of Contents)
1. [ফ্যাসাড আর্কিটেকচার ও ফার্নিচারের বিবরণ (Facade & Interior Design)](#-ফ্যাসাড-আর্কিটেকচার-ও-ফার্নিচারের-বিবরণ-facade--interior-design)
2. [গেমিং FPV কীবোর্ড ও লুক-অ্যারাউন্ড কন্ট্রোল গাইড (Controls Guide)](#-গেমিং-fpv-কীবোর্ড-ও-লুক-অ্যারাউন্ড-কন্ট্রোল-গাইড-controls-guide)
3. [কোডের প্রতিটি ফাংশন ও মেথডের বিস্তারিত বিশ্লেষণ (Function Reference)](#-কোডের-প্রতিটি-ফাংশন-ও-মেথডের-বিস্তারিত-বিশ্লেষণ-function-reference)
4. [ওপেনজিএল ও গ্লুট লাইব্রেরি ফাংশনসমূহের বিবরণ (OpenGL & GLUT API Details)](#-ওপেনজিএল-ও-গ্লুট-লাইব্রেরি-ফাংশনসমূহের-বিবরণ-opengl--glut-api-details)
5. [বিল্ডিং আর্কিটেকচার ও মেজারমেন্ট (Architectural Dimensions)](#-বিল্ডিং-আর্কিটেকচার-ও-মেজারমেন্ট-architectural-dimensions)
6. [রান ও কম্পাইল করার নিয়ম (How to Compile & Run)](#-রান-ও-কম্পাইল-করার-নিয়ম-how-to-compile--run)

---

## 🏛️ ফ্যাসাড আর্কিটেকচার ও ফার্নিচারের বিবরণ (Facade & Interior Design)

### ১. সম্মুখভাগ বা ফ্যাসাডের পরিষ্কার বিভাজন (Window & Balcony Separation):
- **বাম উইং (Left Side Wing: X = -4.8 থেকে -2.0):**
  - শুধুমাত্র **প্রশস্ত কাঁচের জানালা (Large Glass Window)**।
  - গাঢ় ফ্রেম, উজ্জ্বল স্কাই-ব্লু গ্লাস, কাঁচের ওপর আলোর প্রতিফলন স্ট্রাইপ (Highlight), সাদা উইন্ডো গ্রিল/ডিভাইডার এবং উপরে কংক্রিট সানশেড (Chhajja)।
  - এই অংশে কোনো ব্যালকনি নেই।
- **ডান উইং (Right Side Wing: X = +1.0 থেকে +5.0):**
  - শুধুমাত্র **ব্যালকনি (Dedicated Balcony)**।
  - প্রজেক্টিং কংক্রিট স্ল্যাব, স্বচ্ছ ব্লু গ্লাস রেলিং প্যানেল, স্টেইনলেস স্টিলের টপ হ্যান্ডরেইল।
  - ব্যালকনির ভেতরে কোনো জানালা নেই — রুম থেকে সরাসরি ব্যালকনিতে পা রাখার জন্য রয়েছে **ব্যালকনি এন্ট্রান্স ডোর (Balcony Sliding Door)**।

### ২. ফ্লোর অনুযায়ী রুমের আসবাবপত্র (Furniture by Floor):
- **Ground Floor (Living Room & Lounge):** রয়্যাল নেভি ব্লু ৩-সিটার সোফা, কুশন, সাইড আর্মচেয়ার, কফি টেবিল, ওয়ালের ফ্ল্যাট-স্ক্রিন টিভি ইউনিট, ডাইনিং টেবিল ও ৪টি চেয়ার, স্প্লিট এসি, ২টি সিলিং ফ্যান ও লাইট প্যানেল।
- **1st Floor (Master Bedroom):** কিং-সাইজ খাট (গদি, লাল চাদর, ২টি বালিশ ও হেডবোর্ড), ২ পাল্লার কাঠের ওয়ারড্রব, ল্যাপটপ সহ স্টাডি টেবিল-চেয়ার, সিঙ্গেল সোফা, স্প্লিট এসি, ফ্যান ও সিলিং লাইট।
- **2nd Floor (Executive Office):** এক্সিকিউটিভ ডেস্কে রাখা ল্যাপটপ কম্পিউটার, অফিস চেয়ার, ৩ তাকের বইভর্তি লাইব্রেরি বুকশেলফ, লেদার সোফা, গ্লাস টি-টেবিল, এসি, ফ্যান ও লাইট।
- **3rd Floor (Guest Studio):** কুইন-সাইজ বেড, আলমারি, স্টাডি ডেস্ক, লাউঞ্জ সোফা, স্প্লিট এসি, সিলিং ফ্যান ও লাইট।
- **4th Floor (Penthouse Suite):** বিলাসবহুল এল-শেপ হোয়াইট কর্নার সোফা, গ্লাস কফি টেবিল, ৬৫" বড় টিভি কনসোল, বার/হাই ডাইনিং টেবিল-চেয়ার, হাই-ক্যাপাসিটি এসি ও সিলিং লাইট।

---

## 🎮 গেমিং FPV কীবোর্ড ও লুক-অ্যারাউন্ড কন্ট্রোল গাইড (Controls Guide)

| কী (Key) | কাজ (Action) | বিস্তারিত বিবরণ (Description) | মেকানিক্স / ম্যাথ প্রভাব |
| :--- | :--- | :--- | :--- |
| **`LEFT Arrow`** | **Look LEFT** | **এক জায়গায় দাঁড়িয়ে বামে তাকাবে** (Turn head left) | `yaw -= 0.06 rad` (Eye স্থির) |
| **`RIGHT Arrow`**| **Look RIGHT**| **এক জায়গায় দাঁড়িয়ে ডানে তাকাবে** (Turn head right) | `yaw += 0.06 rad` (Eye স্থির) |
| **`UP Arrow`** | **Look UP** | **এক জায়গায় দাঁড়িয়ে উপরে তাকাবে** (ছাদ/ফ্যান/লাইট) | `pitch += 0.06 rad` (Eye স্থির) |
| **`DOWN Arrow`** | **Look DOWN** | **এক জায়গায় দাঁড়িয়ে নিচে তাকাবে** (ফার্নিচার/মেঝে) | `pitch -= 0.06 rad` (Eye স্থির) |
| **`w` / `W`** | **Walk Forward** | যেদিকে তাকানো আছে সেদিকে হেঁটে রুমে ঢুকবে | `eyeX += sin(yaw)*s`, `eyeZ -= cos(yaw)*s` |
| **`s` / `S`** | **Walk Backward**| তাকানোর বিপরীত দিকে রুমে বা বাইরে পিছিয়ে আসবে | `eyeX -= sin(yaw)*s`, `eyeZ += cos(yaw)*s` |
| **`a` / `A`** | **Strafe Left** | বাম দিকে পাশে হেঁটে সরবে | `eyeX -= cos(yaw)*s`, `eyeZ -= sin(yaw)*s` |
| **`d` / `D`** | **Strafe Right** | ডান দিকে পাশে হেঁটে সরবে | `eyeX += cos(yaw)*s`, `eyeZ += sin(yaw)*s` |
| **`r` / `R`** | **Rise UP** | চোখের উচ্চতা উপরে ওঠাবে (ওপরের তলায় যাওয়া) | `eyeY += 0.8` |
| **`f` / `F`** | **Fall DOWN** | চোখের উচ্চতা নিচে নামাবে (গ্রাউন্ড ফ্লোরে নামা) | `eyeY -= 0.8` |
| **`+` / `=`** | **Zoom In** | ক্যামেরা লেন্স অপটিক্যাল জুম ইন হবে (FOV কমবে) | `fov -= 2.0` (সীমা: 12.0°) |
| **`-` / `_`** | **Zoom Out** | ক্যামেরা লেন্স অপটিক্যাল জুম আউট হবে (FOV বাড়বে) | `fov += 2.0` (সীমা: 95.0°) |
| **`0`** | **Reset View** | ক্যামেরাকে মূল ডিফল্ট স্থানে রিসেট করে | `eye=(0,6,28)`, `yaw=0`, `pitch=0` |
| **`ESC`** | **Exit** | অ্যাপ্লিকেশন বন্ধ করে বের হবে | `exit(0)` |

---

## 🛠️ কোডের প্রতিটি ফাংশন ও মেথডের বিস্তারিত বিশ্লেষণ (Function Reference)

### ১. আসবাবপত্র অঙ্কন ফাংশনসমূহ (Furniture Methods):
- `drawBed(x, y, z, width, length, r, g, b)`: কাঠের ফ্রেম, ম্যাট্রেস, রঙিন চাদর, বালিশ ও হেডবোর্ড আঁকে।
- `drawSofa(x, y, z, width, depth, r, g, b)`: সোফার বেস, সিট কুশন, ব্যাকরেস্ট, আর্মরেস্ট ও কুশন বালিশ আঁকে।
- `drawTableAndChairs(x, y, z, w, d)`: কাঠের ৪টি লেগ, টেবিলটপ এবং দুই পাশের ৪টি চেয়ার আঁকে।
- `drawStudyDesk(x, y, z, w, d)`: পড়ার টেবিল, তার ওপর রাখা ল্যাপটপ (স্ক্রিন গ্লো সহ) ও চেয়ার আঁকে।
- `drawAC(x, y, z, width, isSideWall)`: ওয়ালের সাদা স্প্লিট এসি, কুলিং ফ্ল্যাপ ও সবুজ রঙের এলইডি টেম্পারেচার ডিসপ্লে আঁকে।
- `drawCeilingFan(x, y, z)`: সিলিং থেকে নামানো রড, গোল্ডেন মোটর এবং ৪টি ঘুরন্ত কাঠের ব্লেড আঁকে।
- `drawCeilingLight(x, y, z)`: সিলিংয়ে ফিট করা ফ্রেম ও হলুদ-সাদা উজ্জ্বল এলইডি প্যানেল আঁকে।
- `drawBookshelf(x, y, z, w, h, d)`: কাঠের আলমারিতে তাকে তাকে লাল, নীল, সবুজ বই আঁকে।
- `drawEntertainmentUnit(x, y, z, w, h)`: মিডিয়া ক্যাবিনেট ও তার ওপর দাঁড় করানো ফ্ল্যাট-স্ক্রিন টিভি আঁকে।
- `drawFloorFurniture(floor, floorY, ceilY)`: ফ্লোর নম্বর অনুযায়ী নির্দিষ্ট ফার্নিচারগুলো সঠিক অবস্থানে স্থাপন করে।

### ২. আর্কিটেকচারাল মেথডসমূহ:
- `drawGlassWindow(...)`: গ্লাস ফ্রেম, স্কাই ব্লু কাঁচ, রিফ্লেকশন হাইলাইট ও সানশেড আঁকে (বিল্ডিংয়ের বাম পাশে)।
- `drawMainDoor(...)`: মার্বেল সিঁড়ি, সেগুন কাঠের খোদাই করা ডাবল পাল্লা, ব্রাস হ্যান্ডেল ও ক্যানোপি আঁকে।
- `drawBalcony(baseY)`: প্রজেক্টিং স্ল্যাব, কাঁচের রেলিং, স্টিল হ্যান্ডরেইল ও ব্যালকনি ডোর আঁকে (বিল্ডিংয়ের ডান পাশে)।
- `drawStaircaseSystem()`: নিচতলা থেকে শুরু করে ছাদ পর্যন্ত প্রতি ফ্লোরের ধাপ, হ্যান্ডরেইল এবং ল্যান্ডিং আঁকে।
- `drawRooftop()`: ছাদ, প্যারাফেট ওয়াল, মামটি রুম, ওভারহেড পানির ট্যাংক ও অ্যান্টেনা আঁকে।

### ৩. ক্যামেরা ও প্রজেকশন মেথডসমূহ:
- `updateCameraLook()`: $\text{dir} = (\cos\text{pitch}\sin\text{yaw}, \sin\text{pitch}, -\cos\text{pitch}\cos\text{yaw})$ সূত্র দিয়ে লক্ষ্যবিন্দু হিসাব করে।
- `getNormal3p(...)` *(KUET Lab-02 p2)*: ৩টি ভার্টেক্সের ক্রস প্রোডাক্ট ($U \times V$) করে সমতলের লম্ব/নরমাল ভেক্টর বের করে।
- `ownTranslatef(...)` *(KUET Lab-02 p4)*: নিজস্ব ৪x৪ ট্রান্সলেশন ম্যাট্রিক্স তৈরি করে `glMultMatrixf` দিয়ে গুণ করে।
- `display()`: `glClear`, `gluPerspective`, `gluLookAt`, `drawScene` এবং `glutSwapBuffers` চালায়।

---

## 📚 ওপেনজিএল ও গ্লুট লাইব্রেরি ফাংশনসমূহের বিবরণ (OpenGL & GLUT API Details)

ল্যাব শিটের শতভাগ সীমাবদ্ধতা রক্ষার্থে শুধুমাত্র শিটের ফাংশনসমূহ ব্যবহৃত হয়েছে:
- `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)`
- `glMatrixMode(GL_PROJECTION)` & `glMatrixMode(GL_MODELVIEW)`
- `glLoadIdentity()`
- `gluPerspective(fov, aspect, 1.0, 500.0)`
- `gluLookAt(eyeX, eyeY, eyeZ, lookX, lookY, lookZ, 0, 1, 0)`
- `glPushMatrix()` & `glPopMatrix()`
- `glTranslatef`, `glScalef`, `glRotatef`, `glColor3f`, `glNormal3f`
- `glBegin(GL_QUADS)` & `glEnd()`, `glVertex3fv`
- `glEnable(GL_DEPTH_TEST)` & `glEnable(GL_NORMALIZE)`
- `glutPostRedisplay()`, `glutSwapBuffers()`, `glFlush()`

---

## 🚀 রান ও কম্পাইল করার নিয়ম (How to Compile & Run)

### পদ্ধতি ১: Code::Blocks IDE ব্যবহার করে (সুপারিশকৃত)
1. Code::Blocks ওপেন করুন।
2. `Building Desgin.cbp` ফাইলটি ওপেন করে `F9` (Build & Run) চাপুন।

### পদ্ধতি ২: টার্মিনাল / PowerShell থেকে MinGW GCC দিয়ে কম্পাইল
```powershell
& "C:\Program Files\CodeBlocks\MinGW\bin\g++.exe" -Wall -I"C:\Program Files\CodeBlocks\MinGW\x86_64-w64-mingw32\include" -L"C:\Program Files\CodeBlocks\MinGW\x86_64-w64-mingw32\lib" main.cpp -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 -o "Building Desgin.exe"
.\Building Desgin.exe
```
