# Copyright 2004-2006, Todd Burch - Burchwood USA   http://www.burchwoodusa.com 

=begin
Smustard.com(tm) Ruby Script End User License Agreement

This is a License Agreement is between you and Smustard.com.

If you download, acquire or purchase a Ruby Script or any freeware or any other product (collectively "Scripts") from Smustard.com, then you hereby accept and agree to all of the following terms and conditions:

Smustard.com, through its agreements with individual script authors, hereby grants you a permanent, worldwide, non-exclusive, non-transferable, non-sublicensable use license with respect to its rights in the Scripts.

If you are an individual, then you may copy the Scripts onto any computer you own at any location.

If you are an entity, then you may not copy the Scripts onto any other computer unless you purchase a separate license for each computer and you must have a separate license for the use of the Script on each computer.

You may not alter, publish, market, distribute, give, transfer, sell or sublicense the Scripts or any part of the Scripts.

This License Agreement is governed by the laws of the State of Texas and the United States of America.

You agree to submit to the jurisdiction of the Courts in Houston, Harris County, Texas, United States of America, to resolve any dispute, of any kind whatsoever, arising out of, involving or relating to this License Agreement.

THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

This software has not been endorsed or sanctioned by Google.  Any comments, concerns or issues about this software 
or the affects of this software should be not directed to Google, but to Smustard.com.  
=end

# Name :          scrtool.rb 1.1
# Description :   Left Click to reverse any faces.
# Author :        Todd Burch   http://www.burchwoodusa.com 
# Usage :         1. Active the Tool from the Tools Menu
#                 2. Left Click on any face to reverse it.
# Date :          11.Nov.2004
# Type :          Tool
# History:        1.0 (11.Nov.2004) - first version
#                 1.1 (12.May.2006) - minor updates
#                                  
#-----------------------------------------------------------------------------
require 'sketchup.rb'

class ReverseFaceTool 

def initialize
  @ip = Sketchup::InputPoint.new                   # Define the variable. 
  end

def reset
  @ip.clear
  Sketchup::set_status_text "Left Click any Face to Reverse"   # Status Bar hint. 
  end

def activate
  self.reset
  end

def onMouseMove(flags, x, y, view)
  @ip.pick(view,x,y)                    # Get current Screen coordinates.
  view.tooltip = @ip.tooltip            # Set a tooltip if hovering over geometry.
  end

def onLButtonDown(flags, x, y, view)
  @ip.pick(view,x,y)                         # Get the current pick object 
  if (@ip.face) then @ip.face.reverse! end   # if on a face, reverse it
  end

end # class ReverseFaceTool 

UI.menu("Tools").add_item("Left Click Reverse Face Tool") { Sketchup.active_model.select_tool ReverseFaceTool.new } if (not file_loaded?("scrtool.rb")) 

file_loaded("scrtool.rb")

