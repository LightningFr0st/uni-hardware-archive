----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 18:30:31
-- Design Name: 
-- Module Name: store_register_beh - Behavioral
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

entity store_register_async_struct is
    Port ( En : in STD_LOGIC;
           Reset: in STD_LOGIC;
           DIn0 : in STD_LOGIC;
           DIn1 : in STD_LOGIC;
           DOut0 : out STD_LOGIC;
           DOut1 : out STD_LOGIC);
end store_register_async_struct;

architecture Structural of store_register_async_struct is

component d_trigger_async is
     Port ( 
     D : in STD_LOGIC;
     En : in STD_LOGIC;
     Reset : in STD_LOGIC;
     Q : out STD_LOGIC);
end component;

begin

trig_0: d_trigger_async port
map
(
    D=>DIn0,
    En=>En,
    Reset=>Reset,
    Q=>DOut0
);

trig_1: d_trigger_async port
map
(
    D=>DIn1,
    En=>En,
    Reset=>Reset,
    Q=>DOut1
);

end Structural;
