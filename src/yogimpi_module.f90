module yogimpi

include 'yogimpif.h'

contains

  logical function Yogi_IntegerToLogical(in_int)

      integer, intent(in) :: in_int

      if (in_int == 0) then
          Yogi_IntegerToLogical = .false.
      else
          Yogi_IntegerToLogical = .true.
      end if

  end function

  integer function Yogi_LogicalToInteger(in_logical)

      logical, intent(in) :: in_logical

      if (in_logical .eqv. .true.) then
          Yogi_LogicalToInteger = 1
      else
          Yogi_LogicalToInteger = 0
      end if

  end function

end module yogimpi
