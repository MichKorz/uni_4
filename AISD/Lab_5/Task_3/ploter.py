import matplotlib.pyplot as plt
import os
import matplotlib

# Ustawienie stylu wykresów dla lepszej czytelności
# plt.style.use('seaborn-v0_8-whitegrid')
# seaborn-v0_8-whitegrid może nie być dostępny we wszystkich środowiskach, użyjmy 'ggplot' lub domyślnego
try:
    plt.style.use('seaborn-v0_8-v0_8-whitegrid')
except:
    try:
        plt.style.use('ggplot')
    except:
        print("Style 'seaborn-v0_8-whitegrid' oraz 'ggplot' niedostępne. Używam domyślnego stylu Matplotlib.")


def plot_n500_experiments():
    """
    Generuje i zapisuje wykresy dla 5 przebiegów eksperymentu z n=500.
    Każdy wykres pokazuje liczbę porównań kluczy w kolejnych operacjach Extract-Min.
    """
    print("\nGenerowanie wykresów dla n=500...")
    for i in range(1, 6):
        filename = f"n500_run_{i}.txt"
        if not os.path.exists(filename):
            print(f"Plik {filename} nie został znaleziony. Pomijam generowanie wykresu dla tego przebiegu.")
            continue

        operation_indices = []
        comparison_counts = []
        try:
            with open(filename, 'r') as f:
                for line_number, line in enumerate(f):
                    parts = line.strip().split()
                    if len(parts) == 2:
                        try:
                            operation_indices.append(int(parts[0]))
                            comparison_counts.append(int(parts[1]))
                        except ValueError:
                            print(f"Ostrzeżenie: Nieprawidłowy format danych w pliku {filename}, linia {line_number + 1}: '{line.strip()}'. Pomijam linię.")
                    elif line.strip(): # Jeśli linia nie jest pusta, ale ma zły format
                        print(f"Ostrzeżenie: Nieoczekiwany format danych w pliku {filename}, linia {line_number + 1}: '{line.strip()}'. Pomijam linię.")

        except Exception as e:
            print(f"Wystąpił błąd podczas odczytu pliku {filename}: {e}")
            continue

        if not operation_indices or not comparison_counts:
            print(f"Brak danych do wykreślenia w pliku {filename} lub dane są niekompletne. Pomijam.")
            continue

        plt.figure(figsize=(12, 7)) # Zwiększony rozmiar dla lepszej czytelności
        plt.plot(operation_indices, comparison_counts, marker='.', linestyle='-', markersize=5, alpha=0.7)
        
        title = f'Liczba porównań w i-tej operacji Extract-Min (n=500, Przebieg {i})'
        plt.title(title, fontsize=14)
        plt.xlabel('Indeks operacji Extract-Min', fontsize=12)
        plt.ylabel('Liczba porównań kluczy', fontsize=12)
        
        plt.grid(True, which="both", ls="--", alpha=0.7) # Lepsza siatka
        plt.tight_layout() # Automatyczne dopasowanie elementów wykresu
        
        plot_filename = f"n500_run_{i}_plot.png"
        try:
            plt.savefig(plot_filename)
            print(f"Zapisano wykres: {plot_filename}")
        except Exception as e:
            print(f"Nie udało się zapisać wykresu {plot_filename}: {e}")
        plt.close() # Zamknięcie figury, aby zwolnić pamięć

def plot_avg_cost_vs_n():
    """
    Generuje i zapisuje wykres zależności średniego kosztu operacji 
    (całkowita liczba porównań / n) od wartości n.
    """
    print("\nGenerowanie wykresu średniego kosztu w zależności od n...")
    filename = "avg_cost_vs_n.txt"
    if not os.path.exists(filename):
        print(f"Plik {filename} nie został znaleziony. Nie można wygenerować wykresu.")
        return

    n_values = []
    average_costs = []
    try:
        with open(filename, 'r') as f:
            for line_number, line in enumerate(f):
                parts = line.strip().split()
                if len(parts) == 2:
                    try:
                        n_values.append(int(parts[0]))
                        average_costs.append(float(parts[1]))
                    except ValueError:
                        print(f"Ostrzeżenie: Nieprawidłowy format danych w pliku {filename}, linia {line_number + 1}: '{line.strip()}'. Pomijam linię.")
                elif line.strip():
                     print(f"Ostrzeżenie: Nieoczekiwany format danych w pliku {filename}, linia {line_number + 1}: '{line.strip()}'. Pomijam linię.")
    except Exception as e:
        print(f"Wystąpił błąd podczas odczytu pliku {filename}: {e}")
        return

    if not n_values or not average_costs:
        print(f"Brak danych do wykreślenia w pliku {filename} lub dane są niekompletne. Pomijam.")
        return

    plt.figure(figsize=(12, 7))
    plt.plot(n_values, average_costs, marker='o', linestyle='-', markersize=5, color='dodgerblue')
    
    title = 'Średni koszt operacji (całkowite porównania / n) w zależności od n'
    plt.title(title, fontsize=14)
    plt.xlabel('Rozmiar wejścia n (liczba elementów wstawionych do jednego kopca)', fontsize=12)
    plt.ylabel('Średnia liczba porównań / n', fontsize=12)
    
    # Można rozważyć skalę logarytmiczną, jeśli zakres wartości jest duży
    # plt.xscale('log')
    # plt.yscale('log')
    
    plt.grid(True, which="both", ls="--", alpha=0.7)
    plt.tight_layout()
    
    plot_filename = "avg_cost_vs_n_plot.png"
    try:
        plt.savefig(plot_filename)
        print(f"Zapisano wykres: {plot_filename}")
    except Exception as e:
        print(f"Nie udało się zapisać wykresu {plot_filename}: {e}")
    plt.close()

if __name__ == "__main__":
    # Upewnienie się, że używany jest backend nieinteraktywny, jeśli skrypt nie jest uruchamiany w środowisku z GUI
    if os.environ.get('DISPLAY','') == '':
        print('Nie wykryto środowiska graficznego, używam backendu Agg dla Matplotlib.')
        matplotlib.use('Agg')

    print("Rozpoczęcie generowania wykresów na podstawie danych z C++...")
    plot_n500_experiments()
    plot_avg_cost_vs_n()
    print("\nZakończono generowanie wszystkich wykresów. Sprawdź pliki .png w bieżącym katalogu.")

