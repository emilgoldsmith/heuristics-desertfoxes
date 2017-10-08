#!/bin/bash
ps au | grep make | awk '{print $2}' | xargs kill
