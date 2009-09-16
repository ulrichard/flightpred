! Copyright 2005-2007 ECMWF
! 
! Licensed under the GNU Lesser General Public License which
! incorporates the terms and conditions of version 3 of the GNU
! General Public License.
! See LICENSE and gpl-3.0.txt for details.
!
!
!  Description: How to create and use and index to access messages from a file
!
!
!  Author: Enrico Fucile 
!
!
program index
  use grib_api
  implicit none
  
  integer              :: iret
  character(len = 256) :: error
  integer,dimension(:),allocatable :: paramId,step,level,number
  integer                          :: oparamId,ostep,olevel,onumber
  integer                          :: paramIdSize,numberSize,levelSize,stepSize
  integer              :: i,j,k,l
  integer              :: idx,igrib,count

  ! create an index from a grib file using some keys
  call grib_index_create(idx,'../../data/index.grib','paramId,number,level,step')

  ! get the number of distinct values of paramId in the index
  call grib_index_get_size(idx,'paramId',paramIdSize)
  ! allocate the array to contain the list of distinct paramId
  allocate(paramId(paramIdSize))
  ! get the list of distinct paramId from the index
  call grib_index_get(idx,'paramId',paramId)
  write(*,'(a,i3)') 'paramIdSize=',paramIdSize

  ! get the number of distinct values of number in the index
  call grib_index_get_size(idx,'number',numberSize)
  ! allocate the array to contain the list of distinct numbers
  allocate(number(numberSize))
  ! get the list of distinct numbers from the index
  call grib_index_get(idx,'number',number)
  write(*,'(a,i3)') 'numberSize=',numberSize

  ! get the number of distinct values of level in the index
  call grib_index_get_size(idx,'level',levelSize)
  ! allocate the array to contain the list of distinct levels
  allocate(level(levelSize))
  ! get the list of distinct levels from the index
  call grib_index_get(idx,'level',level)
  write(*,'(a,i3)') 'levelSize=',levelSize

  ! get the number of distinct values of step in the index
  call grib_index_get_size(idx,'step',stepSize)
  ! allocate the array to contain the list of distinct steps
  allocate(step(stepSize))
  ! get the list of distinct steps from the index
  call grib_index_get(idx,'step',step)
  write(*,'(a,i3)') 'stepSize=',stepSize

  count=0
  do i=1,paramIdSize ! loop on paramId
    ! select paramId=paramId(i)
    call grib_index_select(idx,'paramId',paramId(i))

    do j=1,numberSize ! loop on number
      ! select number=number(j)
      call grib_index_select(idx,'number',number(j))

      do k=1,levelSize ! loop on level
        ! select level=level(k)
        call grib_index_select(idx,'level',level(k))

        do l=1,stepSize ! loop on step
          ! select step=step(l)
          call grib_index_select(idx,'step',step(l))

          call grib_new_from_index(idx,igrib, iret)
          do while (iret /= GRIB_END_OF_INDEX)
             count=count+1
             call grib_get(igrib,'paramId',oparamId)
             call grib_get(igrib,'number',onumber)
             call grib_get(igrib,'level',olevel)
             call grib_get(igrib,'step',ostep)
             write(*,'(A,i3,A,i2,A,i4,A,i3)') 'paramId=',oparamId,&
                     '   number=',onumber,&
                     '   level=' ,olevel, &
                     '   step='  ,ostep
  
             call grib_new_from_index(idx,igrib, iret)
          end do

        end do ! loop on step
      end do ! loop on level
    end do ! loop on number
  end do ! loop on paramId
  write(*,'(i4,a)') count,' messages selected'
  call grib_index_release(idx)

end program index

