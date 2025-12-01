----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 19:49:00
-- Design Name: 
-- Module Name: shift_register_async_struct - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity shift_register_async_struct is
    Port ( DIn : in STD_LOGIC;
           SE : in STD_LOGIC;
           Reset : in STD_LOGIC;
           DOut0 : out STD_LOGIC;
           DOut1 : out STD_LOGIC);
end shift_register_async_struct;

architecture Structural of shift_register_async_struct is
component d_trigger_async is
     Port ( 
     D : in STD_LOGIC;
     En : in STD_LOGIC;
     Reset : in STD_LOGIC;
     Q : out STD_LOGIC);
end component;


signal t0_buff:std_logic;
begin

trig_0: d_trigger_async port
map
(
    D=>DIn,
    En=>SE,
    Reset=>Reset,
    Q=>t0_buff
);

trig_1: d_trigger_async port
map
(
    D=>t0_buff,
    En=>SE,
    Reset=>Reset,
    Q=>DOut1
);

DOut0<=t0_buff;

end Structural;
