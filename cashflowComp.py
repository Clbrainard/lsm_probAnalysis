import math

def ConvToN():
    with open("stops.csv", "r") as f:
        rows = [[float(v) for v in line.strip().split(",")] for line in f if line.strip()]

    rows = [[-1 if v < 0 else int(round(v * 320)) for v in row] for row in rows]

    with open("stops.csv", "w") as f:
        for row in rows:
            f.write(",".join(str(v) for v in row) + "\n")


def load_gbm(filepath):
    rows = []
    with open(filepath, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            values = line.split("\t", 1)[-1]
            rows.append([float(v) for v in values.split(",")])
    return rows


def computeCashflows():
    with open("stops.csv", "r") as f:
        stops = [[int(v) for v in line.strip().split(",")] for line in f if line.strip()]

    gbm = load_gbm("GBM1.csv")

    K = 100
    r = 0.05
    dt = 1 / 320

    cashflows = []
    for row in stops:
        cf_row = []
        for path_idx, idx in enumerate(row):
            if idx < 0:
                cf_row.append(0.0)
                continue
            idx = min(idx, len(gbm[path_idx]) - 1)
            underlying = gbm[path_idx][idx]
            payoff = max(K - underlying, 0)
            discounted = payoff * math.exp(-r * idx * dt)
            cf_row.append(discounted)
        cashflows.append(cf_row)

    with open("cashflows.csv", "w") as f:
        for row in cashflows:
            f.write(",".join(f"{v:.6f}" for v in row) + "\n")

ConvToN()
computeCashflows()