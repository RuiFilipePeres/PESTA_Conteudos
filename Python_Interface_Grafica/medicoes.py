import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, CheckButtons
from threading import Thread
from collections import deque
import time

# --- CONFIGURAÇÕES ---
PORTA = 'COM3'
BAUD = 3_000_000
BUFFER_SIZE = 512
FS = 48000
HEADER = b'\xAA\x55'
VREF = 3.3
BITS_ADC = 12
FFT_MIN_DB = -70
FFT_MAX_DB = 0

# --- BUFFERS CIRCULARES ---
max_samples = 2000
canal0_buffer = deque(maxlen=max_samples)
canal1_buffer = deque(maxlen=max_samples)

# --- FFT MÉDIA MÓVEL ---
fft_avg0 = None
fft_avg1 = None
ema_alpha = 0.9
use_agc = True

# --- CONVERTE FREQ PARA NOTAS MUSICAIS (mais próxima) ---
def freq_to_note(f):
    if f <= 0:
        return "-"
    A4 = 440.0
    notas = ['DO', 'DO#', 'RE', 'RE#', 'MI', 'FA', 'FA#', 'SOL', 'SOL#', 'LA', 'LA#', 'SI']
    n = 12 * np.log2(f / A4)
    i = int(round(n)) + 69
    nota = notas[i % 12]
    oitava = i // 12 - 1
    return f"{nota}{oitava}"

# --- SETUP DO PLOT ---
plt.ion()
fig = plt.figure(figsize=(15, 7))
gs = fig.add_gridspec(2, 2, height_ratios=[3, 1])
ax1 = fig.add_subplot(gs[0, 0])
ax2 = fig.add_subplot(gs[0, 1])
ax3 = fig.add_subplot(gs[1, :])
plt.subplots_adjust(bottom=0.15, right=0.85, hspace=0.4)

line0, = ax1.plot([], [], label='Audio Original', alpha=0.8)
line1, = ax1.plot([], [], label='Filtro', alpha=0.8)
ax1.set_ylabel('Amplitude (V)')
ax1.set_xlabel('Tempo (s)')
ax1.set_title('Sinais no Tempo')
ax1.legend()
ax1.grid(True)

fft_line0, = ax2.plot([], [], label='Audio Original')
fft_line1, = ax2.plot([], [], label='Filtro')
ax2.set_xlim(20, FS / 2)
ax2.set_ylim(FFT_MIN_DB, FFT_MAX_DB)
ax2.set_ylabel('Magnitude (dB)')
ax2.set_xlabel('Frequência (Hz)')
ax2.set_title('Espectro (FFT)')
ax2.legend(loc='center left', bbox_to_anchor=(1.01, 0.5))
ax2.grid(True)

diff_line, = ax3.plot([], [], 'g-', label='Diferença (Filtro - Original)')
ax3.set_ylabel('Diferença (dB)', color='g')
ax3.set_xlabel('Frequência (Hz)')
ax3.set_title('Resposta do Filtro')
ax3.grid(True)
ax3.set_xlim(20, FS / 2)
ax3.set_ylim(-30, 30)
ax3.tick_params(axis='y', labelcolor='g')

freq_text = ax2.text(0.98, 0.95, "", transform=ax2.transAxes,
                     ha='right', va='top', fontsize=10, color='blue')

ax_slider = plt.axes([0.25, 0.05, 0.5, 0.03])
slider = Slider(ax_slider, 'Suavização FFT', 0.0, 0.99, valinit=ema_alpha, valstep=0.01)

ax_check = plt.axes([0.25, 0.01, 0.1, 0.03])
checkbox = CheckButtons(ax_check, ['AGC'], [use_agc])

def update_alpha(val):
    global ema_alpha
    ema_alpha = slider.val

def update_agc(label):
    global use_agc
    use_agc = not use_agc

slider.on_changed(update_alpha)
checkbox.on_clicked(update_agc)

hanning_window = np.hanning(BUFFER_SIZE // 2)

# --- THREAD DE LEITURA SÉRIE ---
def serial_thread():
    try:
        ser = serial.Serial(PORTA, BAUD, timeout=0.5)
        print(f"Conectado à porta {PORTA}")

        while True:
            if ser.read(1) != b'\xAA': continue
            if ser.read(1) != b'\x55': continue

            dados = ser.read(BUFFER_SIZE)
            if len(dados) != BUFFER_SIZE:
                continue

            amostras = np.frombuffer(dados, dtype=np.uint16)
            if len(amostras) % 2 != 0:
                continue

            canal0 = amostras[0::2].astype(np.float32)
            canal1 = amostras[1::2].astype(np.float32)

            escala = VREF / (2**BITS_ADC - 1)
            canal0_volts = (canal0 - np.mean(canal0)) * escala
            canal1_volts = (canal1 - np.mean(canal1)) * escala

            canal0_buffer.extend(canal0_volts)
            canal1_buffer.extend(canal1_volts)

    except Exception as e:
        print(f"Erro na leitura: {e}")

thread = Thread(target=serial_thread, daemon=True)
thread.start()

# --- VARIÁVEIS DE SNR ---
pot_sinal = None
pot_ruido = None
snr_medido = False

def on_key(event):
    global pot_sinal, pot_ruido, snr_medido

    if event.key.upper() == 'S':
        if len(canal0_buffer) >= BUFFER_SIZE // 2:
            sinal = np.array(canal0_buffer)[-BUFFER_SIZE // 2:]
            pot_sinal = np.mean(sinal**2)
            print(f"Potência do sinal armazenada: {pot_sinal:.6e} V²")

    elif event.key.upper() == 'R':
        if len(canal0_buffer) >= BUFFER_SIZE // 2:
            ruido = np.array(canal0_buffer)[-BUFFER_SIZE // 2:]
            pot_ruido = np.mean(ruido**2)
            print(f"Potência do ruído armazenada: {pot_ruido:.6e} V²")

    if pot_sinal is not None and pot_ruido is not None and not snr_medido:
        if pot_ruido == 0:
            print("Erro: potência do ruído é zero.")
        else:
            snr_db = 10 * np.log10(pot_sinal / pot_ruido)
            print(f"\n📊 SNR experimental = {snr_db:.2f} dB")
            with open("snr_resultados.txt", "w") as f:
                f.write(f"Potência do sinal: {pot_sinal:.6e} V²\n")
                f.write(f"Potência do ruído: {pot_ruido:.6e} V²\n")
                f.write(f"SNR: {snr_db:.2f} dB\n")
            snr_medido = True

fig.canvas.mpl_connect('key_press_event', on_key)

# --- LOOP PRINCIPAL ---
T_UPDATE = 0.2
last_freq_update = time.time()
freq_dominante_atual = 0.0

while True:
    if len(canal0_buffer) < BUFFER_SIZE // 2:
        plt.pause(0.05)
        continue

    N = BUFFER_SIZE // 2
    janela = hanning_window[:N]
    sinal0 = np.array(canal0_buffer)[-N:] * janela
    sinal1 = np.array(canal1_buffer)[-N:] * janela

    fft0 = np.abs(np.fft.fft(sinal0))[:N // 2]
    fft1 = np.abs(np.fft.fft(sinal1))[:N // 2]
    freq = np.fft.fftfreq(N, 1 / FS)[:N // 2]

    fft_avg0 = ema_alpha * fft_avg0 + (1 - ema_alpha) * fft0 if fft_avg0 is not None else fft0
    fft_avg1 = ema_alpha * fft_avg1 + (1 - ema_alpha) * fft1 if fft_avg1 is not None else fft1

    if use_agc:
        max_ref = np.max(fft_avg0) + 1e-12
        fft_db0 = 20 * np.log10(fft_avg0 / max_ref + 1e-12)
        fft_db1 = 20 * np.log10(fft_avg1 / max_ref + 1e-12)
    else:
        full_scale = (2**BITS_ADC/2)
        fft_db0 = np.clip(20 * np.log10(fft_avg0 / full_scale + 1e-12), FFT_MIN_DB, FFT_MAX_DB)
        fft_db1 = np.clip(20 * np.log10(fft_avg1 / full_scale + 1e-12), FFT_MIN_DB, FFT_MAX_DB)

    fft_line0.set_data(freq, fft_db0)
    fft_line1.set_data(freq, fft_db1)

    fft_diff = fft_db1 - fft_db0
    diff_line.set_data(freq, fft_diff)
    ax3.relim()
    ax3.autoscale_view(scalex=False, scaley=True)

    now = time.time()
    if now - last_freq_update >= T_UPDATE:
        freq_dominante_atual = freq[np.argmax(fft0)]
        nota = freq_to_note(freq_dominante_atual)
        freq_text.set_text(f"{freq_dominante_atual:.1f} Hz ({nota})")
        last_freq_update = now

    if freq_dominante_atual > 0:
        periodo = int(FS / freq_dominante_atual)
        N_plot = min(3 * periodo, len(canal0_buffer))
    else:
        N_plot = min(300, len(canal0_buffer))

    y0 = np.array(canal0_buffer)[-N_plot:]
    y1 = np.array(canal1_buffer)[-N_plot:]
    t = np.arange(-N_plot, 0) / FS

    line0.set_data(t, y0)
    line1.set_data(t, y1)
    ax1.set_xlim(t[0], t[-1])
    ax1.relim()
    ax1.autoscale_view(scalex=False, scaley=True)

    fig.canvas.draw()
    fig.canvas.flush_events()
    plt.pause(0.001)
