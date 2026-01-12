#!/usr/bin/env python3
"""
plot_benchmark_results.py
Скрипт для визуализации результатов бенчмарка сортировок очередей
Все файлы сохраняются в папку benchmark_results/
"""

import json
import matplotlib.pyplot as plt
import numpy as np
import os
import sys
import glob
import shutil
from datetime import datetime

class BenchmarkVisualizer:
    """Класс для визуализации результатов бенчмарка"""
    
    def __init__(self, json_file=None):
        self.benchmark_dir = "benchmark_results"
        self.json_file = json_file
        self.data = None
        self.sizes = None
        self.selection_times = None
        self.quick_times = None
        self.ratios = None
        self.timestamp = None
        
    def find_latest_benchmark(self):
        """Находит последний JSON файл с результатами"""
        if not os.path.exists(self.benchmark_dir):
            print(f"❌ Папка '{self.benchmark_dir}' не найдена.")
            return None
        
        # Ищем JSON файлы
        json_files = glob.glob(os.path.join(self.benchmark_dir, "benchmark_*.json"))
        if not json_files:
            print(f"❌ В папке '{self.benchmark_dir}' не найдено JSON файлов.")
            return None
        
        # Сортируем по времени создания (по имени файла)
        json_files.sort(reverse=True)
        latest_file = json_files[0]
        
        # Проверяем файл с последним запуском
        last_file_path = os.path.join(self.benchmark_dir, "last_benchmark.txt")
        if os.path.exists(last_file_path):
            with open(last_file_path, 'r') as f:
                last_file = f.read().strip()
                if os.path.exists(last_file):
                    latest_file = last_file
        
        return latest_file
    
    def load_data(self, json_file=None):
        """Загрузка данных из JSON файла"""
        if json_file:
            self.json_file = json_file
        elif not self.json_file:
            self.json_file = self.find_latest_benchmark()
        
        if not self.json_file:
            print("   Сначала запустите программу с параметром --benchmark-auto")
            return False
        
        if not os.path.exists(self.json_file):
            print(f"❌ Файл '{self.json_file}' не найден.")
            return False
        
        try:
            with open(self.json_file, 'r', encoding='utf-8') as f:
                self.data = json.load(f)
            
            # Извлекаем данные
            self.sizes = np.array(self.data['sizes'])
            self.selection_times = np.array(self.data['selection_sort'])
            self.quick_times = np.array(self.data['quick_sort'])
            self.ratios = np.array(self.data['ratios'])
            self.timestamp = self.data.get('timestamp', 
                                          datetime.now().strftime("%Y%m%d_%H%M%S"))
            
            # Очищаем данные от некорректных значений
            self._clean_data()
            
            print("✅ Данные успешно загружены")
            print(f"   • Файл: {os.path.basename(self.json_file)}")
            print(f"   • Размеров данных: {len(self.sizes)}")
            print(f"   • Диапазон: от {self.sizes[0]} до {self.sizes[-1]} элементов")
            return True
            
        except Exception as e:
            print(f"❌ Ошибка чтения файла: {e}")
            return False
    
    def _clean_data(self):
        """Очистка данных от некорректных значений"""
        # Заменяем NaN, Inf и отрицательные значения
        self.selection_times = np.where(
            np.isfinite(self.selection_times) & (self.selection_times >= 0),
            self.selection_times,
            0.000001
        )
        
        self.quick_times = np.where(
            np.isfinite(self.quick_times) & (self.quick_times >= 0),
            self.quick_times,
            0.000001
        )
        
        # Пересчитываем отношения с проверкой деления на ноль
        valid_mask = (self.quick_times > 0.000001)
        self.ratios = np.where(
            valid_mask,
            self.selection_times / self.quick_times,
            0.0
        )
        
        # Ограничиваем слишком большие значения
        self.ratios = np.where(
            np.isfinite(self.ratios),
            np.minimum(self.ratios, 10000),
            self.ratios
        )
    
    def print_summary(self):
        """Вывод сводной информации о результатах"""
        print("\n" + "="*70)
        print("СВОДКА РЕЗУЛЬТАТОВ БЕНЧМАРКА")
        print("="*70)
        
        # Таблица результатов
        print(f"\n{'Размер':>10} | {'Выбор (сек)':>12} | {'Быстрая (сек)':>12} | {'Отношение':>10}")
        print("-" * 60)
        
        for i in range(len(self.sizes)):
            sel_time = self.selection_times[i]
            quick_time = self.quick_times[i]
            ratio = self.ratios[i]
            
            # Форматируем значения
            sel_str = f"{sel_time:>12.6f}" if sel_time >= 0.00001 else "< 0.00001"
            quick_str = f"{quick_time:>12.6f}" if quick_time >= 0.00001 else "< 0.00001"
            ratio_str = f"{ratio:>10.2f}" if ratio > 0 else "N/A"
            
            print(f"{self.sizes[i]:>10} | {sel_str} | {quick_str} | {ratio_str}")
        
        # Статистика
        self._print_statistics()
    
    def _print_statistics(self):
        """Вывод статистической информации"""
        valid_ratios = self.ratios[self.ratios > 0]
        if len(valid_ratios) > 0:
            print("\n" + "="*70)
            print("СТАТИСТИЧЕСКИЙ АНАЛИЗ")
            print("="*70)
            
            print(f"\nСортировка выбором (O(n²)):")
            print(f"   • Минимум: {np.min(self.selection_times):.6f} сек")
            print(f"   • Максимум: {np.max(self.selection_times):.6f} сек")
            print(f"   • Среднее: {np.mean(self.selection_times):.6f} сек")
            
            print(f"\nБыстрая сортировка (O(n log n)):")
            print(f"   • Минимум: {np.min(self.quick_times):.6f} сек")
            print(f"   • Максимум: {np.max(self.quick_times):.6f} сек")
            print(f"   • Среднее: {np.mean(self.quick_times):.6f} сек")
            
            print(f"\nОтношение времени (выбор/быстрая):")
            print(f"   • Минимум: {np.min(valid_ratios):.2f}x")
            print(f"   • Максимум: {np.max(valid_ratios):.2f}x")
            print(f"   • Среднее: {np.mean(valid_ratios):.2f}x")
            
            # Анализ роста
            if len(self.sizes) > 1:
                self._print_growth_analysis()
    
    def _print_growth_analysis(self):
        """Анализ роста времени выполнения"""
        last = -1
        first_mid = len(self.sizes) // 3
        second_mid = 2 * len(self.sizes) // 3
        
        sizes_to_check = [first_mid, second_mid, last]
        
        print(f"\nАНАЛИЗ РОСТА ВРЕМЕНИ:")
        for i in range(1, len(sizes_to_check)):
            idx1 = sizes_to_check[i-1]
            idx2 = sizes_to_check[i]
            
            size_growth = self.sizes[idx2] / self.sizes[idx1]
            time_growth_selection = self.selection_times[idx2] / self.selection_times[idx1]
            time_growth_quick = self.quick_times[idx2] / self.quick_times[idx1]
            
            print(f"\n  От {self.sizes[idx1]:,} до {self.sizes[idx2]:,} элементов "
                  f"({size_growth:.1f}x):")
            print(f"    • Выбор: {time_growth_selection:.1f}x "
                  f"(ожидается ~{size_growth**2:.1f}x для O(n²))")
            print(f"    • Быстрая: {time_growth_quick:.1f}x "
                  f"(ожидается ~{size_growth*np.log(self.sizes[idx2])/np.log(self.sizes[idx1]):.1f}x для O(n log n))")
    
    def create_plots(self):
        """Создание и сохранение графиков"""
        print("\n" + "="*70)
        print("СОЗДАНИЕ ГРАФИКОВ")
        print("="*70)
        
        # Фильтруем данные для графиков
        mask = (self.selection_times > 0.00001) & (self.quick_times > 0.00001)
        if np.sum(mask) < 3:
            print("⚠️  Слишком мало данных для построения графиков")
            return []
        
        valid_sizes = self.sizes[mask]
        valid_selection = self.selection_times[mask]
        valid_quick = self.quick_times[mask]
        valid_ratios = self.ratios[mask]
        
        # Настройка стиля
        plt.style.use('seaborn-v0_8-darkgrid')
        
        # Создаём фигуру с 4 подграфиками
        fig = plt.figure(figsize=(16, 10))
        
        # 1. Основной график: время выполнения
        ax1 = plt.subplot(2, 2, 1)
        ax1.plot(valid_sizes, valid_selection, 'o-', linewidth=2, 
                markersize=6, label='Сортировка выбором', color='#E74C3C')
        ax1.plot(valid_sizes, valid_quick, 's-', linewidth=2,
                markersize=6, label='Быстрая сортировка', color='#3498DB')
        
        ax1.set_xlabel('Размер очереди (элементов)', fontsize=11)
        ax1.set_ylabel('Время выполнения (секунды)', fontsize=11)
        ax1.set_title('Время сортировки в зависимости от размера данных', 
                     fontsize=13, fontweight='bold')
        ax1.legend(fontsize=10, loc='upper left')
        ax1.grid(True, alpha=0.3)
        
        # 2. Логарифмический график
        ax2 = plt.subplot(2, 2, 2)
        ax2.loglog(valid_sizes, valid_selection, 'o-', linewidth=2,
                  markersize=6, label='Сортировка выбором', color='#E74C3C')
        ax2.loglog(valid_sizes, valid_quick, 's-', linewidth=2,
                  markersize=6, label='Быстрая сортировка', color='#3498DB')
        
        # Теоретические линии тренда
        if len(valid_sizes) > 2:
            x_fit = np.linspace(valid_sizes[0], valid_sizes[-1], 100)
            
            # Для O(n²)
            coeff_n2 = np.mean(valid_selection / (valid_sizes ** 2))
            ax2.loglog(x_fit, coeff_n2 * x_fit**2, '--', alpha=0.5,
                      label='O(n²) тренд', color='#E74C3C')
            
            # Для O(n log n)
            coeff_nlogn = np.mean(valid_quick / (valid_sizes * np.log(valid_sizes + 1)))
            ax2.loglog(x_fit, coeff_nlogn * x_fit * np.log(x_fit + 1), '--', alpha=0.5,
                      label='O(n log n) тренд', color='#3498DB')
        
        ax2.set_xlabel('Размер очереди (элементов)', fontsize=11)
        ax2.set_ylabel('Время выполнения (секунды)', fontsize=11)
        ax2.set_title('Логарифмический масштаб с трендами', 
                     fontsize=13, fontweight='bold')
        ax2.legend(fontsize=9)
        ax2.grid(True, alpha=0.3, which='both')
        
        # 3. График отношений
        ax3 = plt.subplot(2, 2, 3)
        if len(valid_ratios) > 0:
            bars = ax3.bar(range(len(valid_ratios)), valid_ratios, 
                          color=['#2ECC71' if r < 10 else '#F39C12' for r in valid_ratios],
                          alpha=0.7)
            
            ax3.axhline(y=1, color='gray', linestyle='--', alpha=0.5)
            if np.mean(valid_ratios) > 0:
                ax3.axhline(y=np.mean(valid_ratios), color='red', linestyle='-', 
                           alpha=0.5, label=f'Среднее: {np.mean(valid_ratios):.1f}x')
            
            ax3.set_xlabel('Размер данных', fontsize=11)
            ax3.set_ylabel('Отношение времени (выбор / быстрая)', fontsize=11)
            ax3.set_title('Во сколько раз сортировка выбором медленнее', 
                         fontsize=13, fontweight='bold')
            ax3.set_xticks(range(len(valid_sizes)))
            ax3.set_xticklabels([f'{s:,}' for s in valid_sizes], rotation=45, fontsize=9)
            ax3.legend(fontsize=9)
            ax3.grid(True, alpha=0.3, axis='y')
            
            # Подписи на столбцах
            for i, (bar, ratio) in enumerate(zip(bars, valid_ratios)):
                if ratio > 0:
                    height = bar.get_height()
                    ax3.text(bar.get_x() + bar.get_width()/2., height + 0.2,
                            f'{ratio:.1f}x', ha='center', va='bottom', fontsize=8)
        else:
            ax3.text(0.5, 0.5, 'Недостаточно данных\nдля сравнения',
                    ha='center', va='center', fontsize=12)
            ax3.set_title('Отношение времени сортировок', fontsize=13, fontweight='bold')
        
        # 4. График производительности на элемент
        ax4 = plt.subplot(2, 2, 4)
        
        time_per_element_selection = valid_selection / valid_sizes
        time_per_element_quick = valid_quick / valid_sizes
        
        ax4.semilogy(valid_sizes, time_per_element_selection, 'o-', linewidth=2,
                    markersize=6, label='Выбор (на элемент)', color='#E74C3C')
        ax4.semilogy(valid_sizes, time_per_element_quick, 's-', linewidth=2,
                    markersize=6, label='Быстрая (на элемент)', color='#3498DB')
        
        ax4.set_xlabel('Размер очереди (элементов)', fontsize=11)
        ax4.set_ylabel('Время на элемент (секунды)', fontsize=11)
        ax4.set_title('Эффективность алгоритмов (меньше = лучше)', 
                     fontsize=13, fontweight='bold')
        ax4.legend(fontsize=10)
        ax4.grid(True, alpha=0.3)
        
        # Общий заголовок
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        title = f"{self.data.get('title', 'Сравнение алгоритмов сортировки')}\n"
        title += f"Тестирование от: {self.timestamp[:4]}-{self.timestamp[4:6]}-{self.timestamp[6:8]} {self.timestamp[9:11]}:{self.timestamp[11:13]}:{self.timestamp[13:15]}\n"
        title += f"Графики созданы: {timestamp}"
        plt.suptitle(title, fontsize=12, fontweight='bold', y=0.98)
        
        plt.tight_layout()
        
        # Создаём подпапку для этого теста
        test_dir = os.path.join(self.benchmark_dir, f"test_{self.timestamp}")
        if not os.path.exists(test_dir):
            os.makedirs(test_dir)
        
        # Копируем исходный JSON файл в подпапку
        shutil.copy2(self.json_file, os.path.join(test_dir, os.path.basename(self.json_file)))
        
        # Сохраняем графики
        base_filename = os.path.join(test_dir, f"benchmark_plots_{self.timestamp}")
        
        formats = ['png', 'pdf', 'svg']
        saved_files = []
        
        for fmt in formats:
            filename = f"{base_filename}.{fmt}"
            plt.savefig(filename, dpi=300, bbox_inches='tight')
            saved_files.append(filename)
            print(f"   ✅ Сохранено: {os.path.basename(filename)}")
        
        # Показываем график
        plt.show()
        
        return saved_files, test_dir
    
    def generate_report(self, test_dir):
        """Генерация текстового отчета"""
        report_file = os.path.join(test_dir, f"benchmark_report_{self.timestamp}.txt")
        
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write("="*70 + "\n")
            f.write("ОТЧЁТ О РЕЗУЛЬТАТАХ БЕНЧМАРКА СОРТИРОВОК\n")
            f.write("="*70 + "\n\n")
            
            f.write(f"Исходный файл: {os.path.basename(self.json_file)}\n")
            f.write(f"Дата тестирования: {self.timestamp[:4]}-{self.timestamp[4:6]}-{self.timestamp[6:8]} {self.timestamp[9:11]}:{self.timestamp[11:13]}:{self.timestamp[13:15]}\n")
            f.write(f"Дата генерации отчета: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Количество тестов: {len(self.sizes)}\n")
            f.write(f"Диапазон размеров: {self.sizes[0]} - {self.sizes[-1]} элементов\n\n")
            
            f.write("ТАБЛИЦА РЕЗУЛЬТАТОВ\n")
            f.write("-"*60 + "\n")
            f.write(f"{'Размер':>10} | {'Выбор (сек)':>12} | {'Быстрая (сек)':>12} | {'Отношение':>10}\n")
            f.write("-"*60 + "\n")
            
            for i in range(len(self.sizes)):
                sel_time = self.selection_times[i]
                quick_time = self.quick_times[i]
                ratio = self.ratios[i]
                
                # Форматируем значения
                sel_str = f"{sel_time:>12.6f}" if sel_time >= 0.00001 else "< 0.00001"
                quick_str = f"{quick_time:>12.6f}" if quick_time >= 0.00001 else "< 0.00001"
                ratio_str = f"{ratio:>10.2f}" if ratio > 0 else "N/A"
                
                f.write(f"{self.sizes[i]:>10} | {sel_str} | {quick_str} | {ratio_str}\n")
            
            # Анализ результатов
            valid_ratios = self.ratios[self.ratios > 0]
            if len(valid_ratios) > 0:
                f.write("\n" + "="*70 + "\n")
                f.write("АНАЛИЗ РЕЗУЛЬТАТОВ\n")
                f.write("="*70 + "\n\n")
                
                avg_ratio = np.mean(valid_ratios)
                max_ratio = np.max(valid_ratios)
                max_ratio_idx = np.argmax(self.ratios)
                
                f.write("1. ОБЩАЯ ЭФФЕКТИВНОСТЬ:\n")
                f.write(f"   • Быстрая сортировка в среднем в {avg_ratio:.1f} раз быстрее\n")
                f.write(f"   • Максимальное преимущество ({max_ratio:.1f}x) при размере "
                       f"{self.sizes[max_ratio_idx]:,} элементов\n\n")
                
                f.write("2. СЛОЖНОСТЬ АЛГОРИТМОВ:\n")
                f.write("   • Сортировка выбором: O(n²) - квадратичная сложность\n")
                f.write("   • Быстрая сортировка: O(n log n) - логарифмическая сложность\n\n")
                
                f.write("3. РЕКОМЕНДАЦИИ ПО ВЫБОРУ АЛГОРИТМА:\n")
                f.write("   • < 1,000 элементов: разница незначительна, можно использовать любой\n")
                f.write("   • 1,000 - 10,000 элементов: быстрая сортировка предпочтительна\n")
                f.write("   • > 10,000 элементов: всегда использовать быструю сортировку\n\n")
                
                f.write("4. ФАЙЛЫ РЕЗУЛЬТАТОВ:\n")
                f.write(f"   • Исходные данные: {os.path.basename(self.json_file)}\n")
                f.write(f"   • Графики: benchmark_plots_{self.timestamp}.[png/pdf/svg]\n")
                f.write(f"   • Отчет: benchmark_report_{self.timestamp}.txt\n")
        
        print(f"   📝 Текстовый отчет: {os.path.basename(report_file)}")
        return report_file
    
    def list_all_tests(self):
        """Вывод списка всех проведенных тестов"""
        if not os.path.exists(self.benchmark_dir):
            print(f"Папка '{self.benchmark_dir}' не найдена.")
            return []
        
        test_dirs = glob.glob(os.path.join(self.benchmark_dir, "test_*"))
        test_dirs.sort(reverse=True)
        
        if not test_dirs:
            print("Проведенные тесты не найдены.")
            return []
        
        print(f"\n📁 Найдено тестов: {len(test_dirs)}")
        for i, dir_path in enumerate(test_dirs, 1):
            dir_name = os.path.basename(dir_path)
            timestamp = dir_name.replace("test_", "")
            
            # Читаем JSON файл для получения информации
            json_files = glob.glob(os.path.join(dir_path, "benchmark_*.json"))
            if json_files:
                try:
                    with open(json_files[0], 'r') as f:
                        data = json.load(f)
                        sizes = data.get('sizes', [])
                        print(f"\n{i}. {dir_name}")
                        print(f"   • Тестов: {len(sizes)}")
                        print(f"   • Размеры: от {min(sizes)} до {max(sizes)}")
                        print(f"   • Дата: {timestamp[:4]}-{timestamp[4:6]}-{timestamp[6:8]} "
                              f"{timestamp[9:11]}:{timestamp[11:13]}")
                except:
                    print(f"\n{i}. {dir_name} (ошибка чтения данных)")
        
        return test_dirs


def main():
    """Основная функция"""
    print("\n" + "="*70)
    print("📊 ВИЗУАЛИЗАТОР РЕЗУЛЬТАТОВ БЕНЧМАРКА СОРТИРОВОК")
    print("="*70)
    
    # Проверка аргументов командной строки
    if len(sys.argv) > 1:
        if sys.argv[1] == "--list":
            visualizer = BenchmarkVisualizer()
            visualizer.list_all_tests()
            return
        elif sys.argv[1] == "--help":
            print("\nИспользование:")
            print("  python plot_benchmark_results.py          - Визуализация последнего теста")
            print("  python plot_benchmark_results.py --list   - Список всех тестов")
            print("  python plot_benchmark_results.py --help   - Эта справка")
            print("  python plot_benchmark_results.py <файл>   - Визуализация конкретного файла")
            return
        elif sys.argv[1].endswith('.json'):
            json_file = sys.argv[1]
        else:
            print(f"Неизвестный аргумент: {sys.argv[1]}")
            print("Используйте --help для справки")
            return
    else:
        json_file = None
    
    # Проверка зависимостей
    try:
        import matplotlib
        import numpy
    except ImportError:
        print("❌ Ошибка: требуются библиотеки matplotlib и numpy")
        print("   Установите их командой:")
        print("   pip install matplotlib numpy")
        return
    
    # Создаем визуализатор
    visualizer = BenchmarkVisualizer(json_file)
    
    # Загружаем данные
    if not visualizer.load_data():
        return
    
    # Выводим сводку
    visualizer.print_summary()
    
    # Создаем графики
    print("\nСоздание графиков...")
    result = visualizer.create_plots()
    
    if not result:
        print("⚠️  Не удалось создать графики")
        return
    
    saved_plots, test_dir = result
    
    # Генерируем отчет
    print("\nГенерация отчета...")
    report_file = visualizer.generate_report(test_dir)
    
    print("\n" + "="*70)
    print("✅ ВИЗУАЛИЗАЦИЯ ЗАВЕРШЕНА!")
    print("="*70)
    
    print(f"\n📁 Результаты сохранены в папке: {test_dir}")
    print(f"📊 Созданные файлы:")
    print(f"   • JSON с данными: {os.path.basename(visualizer.json_file)}")
    print(f"   • Графики: {len(saved_plots)} файлов в форматах PNG, PDF, SVG")
    print(f"   • Отчет: {os.path.basename(report_file)}")
    
    print("\n📋 Для нового тестирования:")
    print("   1. Запустите: ./program --benchmark-auto")
    print("   2. Запустите этот скрипт снова")
    
    print("\n🔍 Для просмотра всех тестов:")
    print("   python plot_benchmark_results.py --list")


if __name__ == "__main__":
    main()