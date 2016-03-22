DELETE FROM escort_waypoints WHERE entry = 10638;
INSERT INTO escort_waypoints VALUES
(10638, 0, -4903.52, -1368.34, -52.611, 5000, ''),
(10638, 1, -4906, -1367.05, -52.611, 0, '');

UPDATE creature_template SET scriptname = "npc_kanati" WHERE entry = 10638;

DELETE FROM escort_waypoints WHERE entry = 10646;
INSERT INTO escort_waypoints VALUES
(10646, 0, -4792.4, -2137.78, 82.423, 0, ''),
(10646, 1, -4813.51, -2141.54, 80.774, 0, ''),
(10646, 2, -4828.63, -2154.31, 82.074, 0, ''),
(10646, 3, -4833.77, -2149.18, 81.676, 0, ''),
(10646, 4, -4846.42, -2136.05, 77.871, 0, ''),
(10646, 5, -4865.08, -2116.55, 76.483, 0, ''),
(10646, 6, -4888.43, -2090.73, 80.907, 0, ''),
(10646, 7, -4893.07, -2085.47, 82.094, 0, ''),
(10646, 8, -4907.26, -2074.93, 84.437, 5000, ''),
(10646, 9, -4899.9, -2062.14, 83.78, 0, ''),
(10646, 10, -4897.76, -2056.52, 84.184, 0, ''),
(10646, 11, -4888.33, -2033.18, 83.654, 0, ''),
(10646, 12, -4876.34, -2003.92, 90.887, 0, ''),
(10646, 13, -4872.23, -1994.17, 91.513, 0, ''),
(10646, 14, -4879.57, -1976.99, 92.185, 5000, ''),
(10646, 15, -4879.05, -1964.35, 92.001, 0, ''),
(10646, 16, -4874.72, -1956.94, 90.737, 0, ''),
(10646, 17, -4869.47, -1952.61, 89.206, 0, ''),
(10646, 18, -4842.47, -1929, 84.147, 0, ''),
(10646, 19, -4804.44, -1897.3, 89.362, 0, ''),
(10646, 20, -4798.07, -1892.38, 89.368, 0, ''),
(10646, 21, -4779.45, -1882.76, 90.169, 5000, ''),
(10646, 22, -4762.08, -1866.53, 89.481, 0, ''),
(10646, 23, -4766.27, -1861.87, 87.847, 0, ''),
(10646, 24, -4782.93, -1852.17, 78.354, 0, ''),
(10646, 25, -4793.61, -1850.96, 77.658, 0, ''),
(10646, 26, -4803.32, -1855.1, 78.958, 0, ''),
(10646, 27, -4807.97, -1854.5, 77.743, 0, ''),
(10646, 28, -4837.21, -1848.49, 64.488, 0, ''),
(10646, 29, -4884.62, -1840.4, 56.219, 0, ''),
(10646, 30, -4889.71, -1839.62, 54.417, 0, ''),
(10646, 31, -4893.9, -1843.69, 53.012, 0, ''),
(10646, 32, -4903.14, -1872.38, 32.266, 0, ''),
(10646, 33, -4910.94, -1879.86, 29.94, 0, ''),
(10646, 34, -4920.05, -1880.94, 30.597, 0, ''),
(10646, 35, -4924.46, -1881.45, 29.292, 0, ''),
(10646, 36, -4966.12, -1886.03, 10.977, 0, ''),
(10646, 37, -4999.37, -1890.85, 4.43, 0, ''),
(10646, 38, -5007.27, -1891.67, 2.771, 0, ''),
(10646, 39, -5013.33, -1879.59, -1.947, 0, ''),
(10646, 40, -5023.33, -1855.96, -17.103, 0, ''),
(10646, 41, -5038.51, -1825.99, -35.821, 0, ''),
(10646, 42, -5048.73, -1809.8, -46.457, 0, ''),
(10646, 43, -5053.19, -1791.68, -57.186, 0, ''),
(10646, 44, -5062.09, -1794.4, -56.515, 0, ''),
(10646, 45, -5052.66, -1797.04, -54.734, 5000, '');

UPDATE creature_template SET scriptname = "npc_lakota_windsong" WHERE entry = 10646;