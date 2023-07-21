#THIS IS A WAY TO MERGE DIRECTLY THROUGH A GIVEN DIRECTORY, ASSUMING ALL INPUTS HAVE THE SAME NAME WITH A DIFFERENT NUMBER ATTACHED
#Note: writes 2 rows if they are the same

import pandas as pd
import glob
import os

#setting the path for joining multiple files
files = os.path.join('CSV_ARCHIVE', '*.csv')

#list of merged files returned
files = glob.glob(files)

#print("Merged CSV:")

#joining files with concat and read_csv
df = pd.concat(map(pd.read_csv, files), ignore_index = True)
#print(df)


df.to_csv('CSV_STAGED/merged.csv', index = False)




#THIS IS A WAY TO MERGE ALL .CSV FILES IN A FOLDER, ASSUMING THEY ALL HAVE THE SAME COLUMNS
#Note: if one csv file does not have the same columns, it will fail
#Note: if two rows have the same data, it only writes one column in the merge file
'''
import os
import pandas as pd
import numpy as np

df = pd.DataFrame(columns = ['name', 'day', 'number'])

#changes current working directory to the one listed
os.chdir('/home/olpersi/Downloads/merge_testing')


#os.listdir() gets the list of all files and directories in specified directory^
print('Files merged together include:')
for file in os.listdir():
    if file.endswith('.csv'):
        newdf = pd.read_csv(file)
        df = df.merge(newdf, how = 'outer')
        print(file)


df.to_csv('/home/olpersi/Downloads/merge_testing/merge.csv', index = False)
'''




#THIS IS A WAY TO MERGE BY INPUTTING ALL THE FILE NAMES INTO AN ARRAY
'''
import pandas as pd

#reading in the data and merging to one file
files = ['/home/olpersi/Downloads/merge_testing/test1.csv', '/home/olpersi/Downloads/merge_testing/test2.csv']

df = pd.read_csv('/home/olpersi/Downloads/merge_testing/test.csv')

for file in files:
    newdf = pd.read_csv(file)
    df = df.merge(newdf, how = 'outer')

df.to_csv('/home/olpersi/Downloads/merge_testing/merge.csv', index = False)

'''
