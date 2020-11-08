import subprocess
import os 

tmp_game_filename = 'tmp_game.rbg'
tmp_game_text = '''
#players = red(100), blue(50)
#pieces = e, r, b
#variables = turn(10)
#board = rectangle(up, down, left, right,
    [b,r]
    [e,e])

#rules = ->red up* (left + right) (up {e})* up (left + right {e}) up [b] ->blue
'''

with open(tmp_game_filename,'w') as f:
    print(tmp_game_text, file=f)

out = subprocess.getoutput('./print {} --modifier_indices true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(/*1*/->redup*(left+right)(up{e})*up(left+(right{e}))up/*2*/[b]/*3*/->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->redup*(.left+.right)(up{e})*up(.left+.(right{e}))up[b]->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --disable_adding_dots_in_shifttables true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->redup*(left+right)(up{e})*up(.left+.(right{e}))up[b]->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_stars true --add_dots_after_stars true --add_dots_after_alternatives true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->red(.up)*.(left+right).(.(up{e}))*.up(left+(right{e})).up[b]->blue)')

out = subprocess.getoutput('./print {} --add_dots_in_alternatives true --add_dots_in_stars true --add_dots_after_stars true --add_dots_after_alternatives true --disable_adding_dots_in_shifttables true'.format(tmp_game_filename))

assert('#rules' in out)
rules = out[out.find('#rules'):]
rules = rules.replace(' ','').replace('\n','')
assert(rules == '#rules=(->redup*(left+right).(.(up{e}))*.up(.left+.(right{e})).up[b]->blue)')

os.remove(tmp_game_filename)