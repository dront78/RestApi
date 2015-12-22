#!/bin/bash
declare -a methods=('PUT' 'PUT' 'WRONG' 'DELETE')
declare -a fnames=('ivan' 'andrew' 'elena' 'olga')
declare -a lnames=('lyapunov' 'hazi' 'smirnoff' 'doe')
declare -a genders=('u' 'm' 'f')
declare -a ages=('31' '18' '87' '57')

for i in {1..10000}
do
   firstname=${fnames[$RANDOM % ${#fnames[@]} ]}
   lastname=${lnames[$RANDOM % ${#lnames[@]} ]}
   gender=${genders[$RANDOM % ${#genders[@]} ]}
   age=${ages[$RANDOM % ${#ages[@]} ]}
   meth=${methods[$RANDOM % ${#methods[@]} ]}
   curl -X $meth "http://127.0.0.1:8080/user?firstname=$firstname&lastname=$lastname&gender=$gender&age=$age" &>/dev/null &
done

echo http://127.0.0.1:8080/user?firstname=olga
echo http://127.0.0.1:8080/user?firstname=ivan&age=18
echo http://127.0.0.1:8080/user?gender=u
