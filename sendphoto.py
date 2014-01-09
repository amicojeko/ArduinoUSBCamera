# coding=utf-8

# Copyright (C) 2014  Stefano Guglielmetti

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import base64
import sys
from temboo.core.session import TembooSession
from temboo.Library.Facebook.Publishing import UploadPhoto

print str(sys.argv[1])

with open(str(sys.argv[1]), "rb") as image_file:
    encoded_string = base64.b64encode(image_file.read())

session = TembooSession('ACCOUNT_NAME', 'APP_KEY_NAME', 'APP_KEY_VALUE')
uploadPhotoChoreo = UploadPhoto(session)

# Get an InputSet object for the choreo
uploadPhotoInputs = uploadPhotoChoreo.new_input_set()

# Set inputs
uploadPhotoInputs.set_AccessToken("ACCESS_TOKEN")
uploadPhotoInputs.set_Message("MESSAGE")
uploadPhotoInputs.set_AlbumID("ALBUM_ID")
uploadPhotoInputs.set_Photo(encoded_string)

# Execute choreo
uploadPhotoResults = uploadPhotoChoreo.execute_with_results(uploadPhotoInputs)
