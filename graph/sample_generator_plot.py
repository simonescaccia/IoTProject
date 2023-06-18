# Use plt to plot data
# include plt
import matplotlib.pyplot as plt
# include pandas
import pandas as pd

# Constants
X = "Logic time"
SOURCE = "Source"
FORKS0 = "Fork Sensor 0" 
FORKS1 = "Fork Sensor 1"
BRANCH0 = "Branch 0"
BRANCH1 = "Branch 1"

# Read data from sample_generator.txt
data = pd.read_csv('sample_generator_data.csv', sep=",")

# Plot data
data.plot(x=X, y=[SOURCE, FORKS0, BRANCH0, FORKS1, BRANCH1], kind="bar")
plt.show()
plt.grid()
