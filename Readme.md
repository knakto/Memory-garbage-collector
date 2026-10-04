<h1 align="center">
  <br>
  ⚓ Memory Gabage Collector (mem-gbc)
  <br>
</h1>

<h4 align="center">A lightweight, Arena-like Garbage Collector and Memory Tracker in C.</h4>

<p align="center">
  <a href="https://en.wikipedia.org/wiki/C_(programming_language)">
    <img src="https://img.shields.io/badge/Language-C99-blue.svg?style=flat-square&logo=c" alt="Language">
  </a>
  <a href="https://valgrind.org/">
    <img src="https://img.shields.io/badge/Valgrind-0%20Leaks%20%7C%200%20Errors-brightgreen.svg?style=flat-square" alt="Valgrind">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-orange.svg?style=flat-square" alt="License">
  </a>
  <a href="#architecture">
    <img src="https://img.shields.io/badge/Architecture-Singleton%20Linked%20List-purple.svg?style=flat-square" alt="Architecture">
  </a>
</p>

<p align="center">
  <i>🌍 Read this document in other languages:</i><br>
  <a href="assets/README-en.md">🇬🇧 English</a> | <a href="Readme.md">🇹🇭 ภาษาไทย</a>
</p>

<p align="center">
  <a href="#-1-introduction">Introduction</a> •
  <a href="#-2-memory-layout">Memory Layout</a> •
  <a href="#-3-architecture">Architecture</a> •
  <a href="#-4-allocation-flow--ram-overhead">Allocation Flow</a> •
  <a href="#-5-how-to-use">How To Use</a> •
  <a href="#-6-comparison">Comparison</a> •
  <a href="#-7-use-cases">Use Cases</a>
</p>

---

## 🚀 1. Introduction

ในภาษา C ความท้าทายอันดับหนึ่งคือ **Manual Memory Management** ทุกครั้งที่เราจองหน่วยความจำด้วย `malloc()` หน้าที่และความรับผิดชอบตกอยู่ที่ตัวเราทั้งหมด ยิ่งโปรแกรมมีความซับซ้อน มีเงื่อนไขการ branching หรือเกิด Error กลางคัน โอกาสหลุดรั่ว (Memory Leak) หรือการเผลอ `free()` ซ้ำ (Double Free) ย่อมสูงขึ้นเรื่อยๆ

`mem_gbc` ถูกสร้างขึ้นเพื่อขจัดภาระดังกล่าว โดยทำหน้าที่เป็น Wrapper ครอบ `malloc()`:
* **Auto-Tracking:** ทุกก้อนหน่วยความจำที่จองจะถูกบันทึก Address ลง Tracker กลางโดยอัตโนมัติ
* **Single Point Cleanup:** เมื่อโปรแกรมทำงานเสร็จสิ้นหรือเกิด Fatal Error สั่ง `gbc_clear()` เพียงคำสั่งเดียว ตัวระบบจะกวาดคืน Memory ทั้งหมดบน Heap กลับสู่ OS ทันที

---

## 🧠 2. Memory Layout

การทำความเข้าใจ `mem_gbc` ต้องเริ่มจากการมองเห็นภาพว่า OS จัดสรรพื้นที่ให้โปรแกรมของเราอย่างไร:

![Memory Layout](./assets/memory-layout.png)

* **Stack Memory:** จัดการแบบ LIFO (Last-In, First-Out) เร็วมากด้วยการเลื่อน Stack Pointer ใช้เก็บ Local Variables และ Function Call Frames ตัวแปรจะหายไปทันทีเมื่อออกจาก Scope ของฟังก์ชัน จึงไม่สามารถส่งต่อข้อมูลขนาดใหญ่ข้าม Lifecycle ได้
* **Heap Memory:** พื้นที่ขนาดใหญ่ที่โปรแกรมเมอร์ควบคุมเองผ่าน `malloc()` / `free()` ข้อมูลจะคงอยู่ตลอดไปจนกว่าจะสั่งปล่อยคืนระบบ แต่หากทำ Address หายโดยยังไม่ `free()` จะเกิด Memory Leak ทันที
* **Data Segment & BSS (Static / Global Memory):** เก็บตัวแปร Global และตัวแปรที่ประกาศด้วยคีย์เวิร์ด `static` มีอายุตลอดทั้ง Program Lifetime (เกิดขึ้นตั้งแต่โปรแกรมเริ่มรัน และถูกทำลายเมื่อโปรแกรมปิดตัวลงเท่านั้น)

---

## 🏗️ 3. Architecture

ปัญหาคลาสสิกของ Central Memory Tracker คือ *"จะเก็บ Head ของ Tracker ไว้ที่ไหนโดยไม่ต้องสร้าง Global Variable ให้โค้ดสกปรก?"*

ทางออกที่ปลอดภัยและเป็นระเบียบคือการสร้าง **Encapsulated Singleton** ผ่านฟังก์ชัน:

```c
/* Pointer Storage Implementation */
t_lst **global_pointer_storage(void)
{
    static t_lst *storage;

    return (&storage);
}
```

### ทำไมต้องเป็น `static` ภายในฟังก์ชัน?
ตัวแปร `storage` จะถูกจัดเก็บอยู่ใน **BSS Segment** มีอายุตลอดทั้งโปรแกรม การประกาศอยู่ภายใน Scope ฟังก์ชัน ทำให้ไม่มีไฟล์ภายนอกสามารถแอบแก้ไขค่า Pointer นี้ได้โดยตรง ป้องกันปัญหา Data Tampering

### ทำไมต้องคืนค่าเป็น Double Pointer (`t_lst **`)?
ตัวแปร `storage` คือ Head Pointer ของ Linked List (ซึ่งเก็บ Address ของ Node แรก) เมื่อเราต้องการเพิ่ม Node เข้าไปที่ส่วนหัว หรือต้องการสั่ง Reset ให้ `*head = NULL` ฟังก์ชันภายนอกจำเป็นต้องเข้าถึง Address ของตัวชี้ (`&storage`) การส่ง Double Pointer ทำให้ฟังก์ชันอย่าง `lst_addback()` และ `lst_clear()` สามารถแก้ไขค่าของ Pointer ตัวจริงที่อยู่ใน BSS ได้ทันที

---

## ⚙️ 4. Allocation Flow & RAM Overhead

![Tracking Architecture](./assets/linked_list_structure.jpg)

### ทำไมต้อง Wrap ด้วย `gbc_malloc()`?
เมื่อเราเรียก `gbc_malloc(size)` สิ่งที่เกิดขึ้นเบื้องหลังมี 2 ส่วน:
1. ขอพื้นที่จริงบน Heap ให้กับ User Data Block
2. สร้าง `t_lst` Node ขึ้นมาเพื่อเก็บ Address ของบล็อกนั้น แล้วผูกต่อเข้ากับ Tracker

```c
/* แผนผังการไหลของข้อมูลและการจัดสรร Memory */
void *gbc_malloc(size_t size)
{
    void  *user_block = malloc(size);         // [Heap] จองพื้นที่สำหรับ Data จริง
    t_lst *node       = lst_new(user_block);  // [Heap] สร้าง Node เก็บ Address

    lst_addback(global_pointer_storage(), node); // [BSS] ผูกเข้ากับ Anchor Pointer
    return (user_block);
}
```

### คำถามสำคัญ: พื้นที่ Global / BSS จะเต็มหรือไม่?
**คำตอบ: ไม่เต็มแน่นอน**

พื้นที่ในส่วนของ Global / BSS ไม่ได้เก็บข้อมูลทั้งหมด สิ่งที่อยู่ใน BSS มีเพียงแค่ **Single Pointer ขนาด 8 ไบต์ตัวเดียว** นั่นคือ `static t_lst *storage;` (บน 64-bit architecture) ซึ่งทำหน้าที่เป็นเพียง Anchor หรือ "สมอเรือ" ที่คอยชี้ไปหา Node แรก

| Memory Segment | สิ่งที่ถูกจัดเก็บ | พฤติกรรมการขยายตัว |
| :--- | :--- | :--- |
| **BSS Segment** | Anchor Pointer (`static t_lst *storage`) | คงที่เสมอที่ 8 ไบต์ |
| **Heap Segment** | User Blocks + Tracker Linked List Nodes | ขยายตัวตามจำนวน Memory ที่เรียกใช้งานจริง |

---

## 💻 5. How To Use

```c
#include <stdio.h>
#include "mem_gbc.h"

int main(void)
{
    // จองหน่วยความจำผ่าน gbc_malloc เหมือน malloc ปกติ
    char *str = gbc_malloc(32 * sizeof(char));
    int  *arr = gbc_malloc(10 * sizeof(int));

    if (!str || !arr)
    {
        // หากเกิด Error ตรงไหน ไม่ต้องไล่ free ทีละตัว
        gbc_clear();
        return (1);
    }

    // ใช้งานข้อมูลตามปกติ
    snprintf(str, 32, "Hello, mem_gbc!");
    printf("%s\n", str);

    // เคลียร์ทุกอย่างที่เคยจองผ่าน gbc_malloc ทั้งหมดในคำสั่งเดียว
    gbc_clear();
    return (0);
}
```

---

## ⚖️ 6. Comparison

| มิติการเปรียบเทียบ | Standard `malloc()` / `free()` | `mem_gbc` |
| :--- | :--- | :--- |
| **Error Cleanup** | ต้องย้อน free ทุกจุดที่เกิด branching / return | สั่ง `gbc_clear()` ครั้งเดียว คลุมทุกเส้นทาง |
| **Memory Leak Risk** | สูงมาก (หลุดลืม free เพียง 1 path ก็รั่วทันที) | ต่ำมาก (0 leaks หากมีการเรียก `gbc_clear()`) |
| **Double Free Risk** | เกิดขึ้นบ่อยเมื่อโครงสร้างข้อมูลซับซ้อน | ไม่มีทางเกิด เพราะจัดการวงจรชีวิตจากจุดศูนย์กลาง |
| **Code Readability** | รกไปด้วย Boilerplate สำหรับการ cleanup | คลีน มุ่งเน้นไปที่ Core Logic ของโปรแกรม |

---

## 🎯 7. Use Cases

* 🖥️ **CLI Tools & Parsers:** โปรแกรมที่ต้องอ่าน Input สร้าง Data Tree ประมวลผลก้อนใหญ่ แล้วพ่น Output ออกมาก่อน Terminate ตัวเอง
* 🏫 **School & Low-Level Projects (e.g., 42 Network):** โค้ดที่ต้องการรับประกัน **0 leaks และ 0 errors จาก Valgrind** โดยไม่ต้องเขียนโค้ด cleanup ซับซ้อนในทุก `if (err)`
* 🎮 **Loop & Frame-based Processing:** ใช้เป็นแนวคิดคล้าย Arena Allocator หรือ Scratchpad Memory ที่สามารถสั่ง Flush ล้างทิ้งทั้งก้อนเมื่อจบรอบการคำนวณ

---

<p align="center">
  Crafted with care for clean & leak-free C development.
</p>
