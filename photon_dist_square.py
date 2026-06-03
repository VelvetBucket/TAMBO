import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

photons = pd.read_csv("./basic_square-BUILD/my_experiment_photons_merged.csv")
print(photons.head())
print("Shape of the dataset: ", photons.shape)

opticals = photons[photons.Particle == "optical"]
print("Shape of optical dataset: ", opticals.shape)

plt.hist(opticals.Wavelength_nm, bins=40, density=True)
plt.grid(True)
plt.xlabel("Photon wavelength (nm)")
plt.ylabel("Number of photons")
plt.title(f"Wavelength distribution of optical photons\nTotal number of optical photons: {opticals.shape[0]}")
plt.savefig("./photon_distr.png")
plt.show()

