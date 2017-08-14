Import Vector Image Stack Filter {#itkimportvectorimagestack}
=============

## Group (Subgroup) ##
IO (Input)


## Description ##



----------

![Screenshot](Images/ImportVectorStackGUI.png)

## Notes
When importing color images they will be imported as RGBA, or color with Alpha values. Due to some limitations of the XDMF wrapper the 4 component arrays will only show as 3 component arrays in the XDMF description which will mess up the rendering in ParaView. The only current way to solve this issue is to import the image data and then follow that with the [Flatten Image](flattenimage.html) filter which will convert the color data to gray scale data. Then writing out the .dream3d file with the xdmf wrapper will allow the user to properly see their data.


## Parameters ##


| Name             |  Type  |
|------------------|--------|
| Feature Array Name | String |


## Required DataContainers ##

Voxel

## Required Objects ##


None


## Created Objects ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | User Defined       | ....        | other   |



## Authors ##


**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


