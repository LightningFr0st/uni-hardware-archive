----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 20:04:50
-- Design Name: 
-- Module Name: johnson_struct - Structural
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

entity johnson_struct is
    Port ( CLK : in STD_LOGIC;
           DOut0 : out STD_LOGIC;
           DOut1 : out STD_LOGIC;
           Reset : in STD_LOGIC);
end johnson_struct;

architecture Structural of johnson_struct is

component d_trigger_sync is
     Port ( 
     D : in STD_LOGIC;
     CLK : in STD_LOGIC;
     En : in STD_LOGIC;
     Reset : in STD_LOGIC;
     Q : out STD_LOGIC;
     nQ: out std_logic);
end component;

signal last_nQ, t0_out:std_logic;
begin
trig_0: d_trigger_sync port
map
(
    D=>last_nQ,
    En=>'1',
    CLK=>CLK,
    Reset=>Reset,
    Q=>t0_out
);

trig_1: d_trigger_sync port
map
(
    D=>t0_out,
    En=>'1',
    CLK=>CLK,
    Reset=>Reset,
    Q=>DOut1,
    nQ=>last_nQ
);

DOut0<=t0_out;

end Structural;
