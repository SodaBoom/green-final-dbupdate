import random
from random import randrange
from datetime import timedelta
from datetime import datetime

def random_datetime():
  start = datetime.strptime('1/1/1971 1:30 PM', '%m/%d/%Y %I:%M %p')
  end = datetime.strptime('1/1/2022 4:50 AM', '%m/%d/%Y %I:%M %p')
  delta = end - start
  int_delta = (delta.days * 24 * 60 * 60) + delta.seconds
  random_second = randrange(int_delta)
  return start + timedelta(seconds=random_second)

N = 1_000_000

try:
  random.seed(1)
  out_object = open("total-energy_100W.csv",'w')
  for id in range(0, N):
    gmt_create = random_datetime()
    gmt_modified = gmt_create
    user_id = random.randint(1, 100)
    total_energy = random.randint(0, 1000)
    format_str = "{},{},{},{},{}\n".format(id, gmt_create, gmt_modified, user_id, total_energy)
    out_object.write(format_str)
finally:
  out_object.close()

try:
  random.seed(1)
  out_object = open("to-collect-energy_100W.csv",'w')
  for id in range(0, N):
    gmt_create = random_datetime()
    gmt_modified = gmt_create
    user_id = random.randint(1, 100)
    to_collect_energy = random.randint(0, 100)
    status = "null"
    format_str = "{},{},{},{},{},{}\n".format(id, gmt_create, gmt_modified, user_id, to_collect_energy, "null")
    out_object.write(format_str)
finally:
  out_object.close()
