# in micro-seconds, timings copied from spec: https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
min_40 = 0.25
max_40 = 0.55
min_45 = 0.30
max_45 = 0.60

min_80 = 0.65
max_80 = 0.95
min_85 = 0.70
max_85 = 1.00

min_4x = min_45
min_8x = min_85
max_4x = max_40
max_8x = max_80

print(f"min_4x: {min_4x}us, max_4x: {max_4x}us")
print(f"min_8x: {min_8x}us, max_8x: {max_8x}us")

def in_4x_range(t):
    if t >= min_4x and t <= max_4x:
        return True
    else:
        return False

def in_8x_range(t):
    if t >= min_8x and t <= max_8x:
        return True
    else:
        return False

def freq_in_range(f):
    return in_4x_range(3 / f) and in_8x_range(5 / f)


f = 6 # in MHz
fmin = f
fmax = f
step = 0.01

# simply brute-force the minimum and maximum frequency
while freq_in_range(fmin):
    fmin -= step
fmin += step

while freq_in_range(fmax):
    fmax += step
fmax -= step

print(f"f: {f}MHz, is in range: {freq_in_range(f)}")
print(f"f: {fmin}MHz, is in range: {freq_in_range(fmin)}")
print(f"f: {fmax}MHz, is in range: {freq_in_range(fmax)}")
