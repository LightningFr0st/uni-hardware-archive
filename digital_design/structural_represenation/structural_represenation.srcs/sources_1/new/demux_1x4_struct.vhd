----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 10:38:05 PM
-- Design Name: 
-- Module Name: demux_1x4_struct - Structural
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

entity demux_1x4_struct is
    Port ( I : in STD_LOGIC;
           S : in STD_LOGIC_VECTOR(1 downto 0);
           D : out STD_LOGIC_VECTOR(3 downto 0));
end demux_1x4_struct;

architecture Structural of demux_1x4_struct is

component inv is
    Port ( I : in STD_LOGIC; O : out STD_LOGIC );
end component;

component and3 is
    Port ( A : in STD_LOGIC; B : in STD_LOGIC; C : in STD_LOGIC; Q : out STD_LOGIC );
end component;

signal nS0, nS1 : STD_LOGIC;

begin
    U1: inv port map (I => S(0), O => nS0);
    U2: inv port map (I => S(1), O => nS1);
    
    U3: and3 port map (A => I, B => nS1, C => nS0, Q => D(0));
    U4: and3 port map (A => I, B => nS1, C => S(0),  Q => D(1));
    U5: and3 port map (A => I, B => S(1),  C => nS0, Q => D(2));
    U6: and3 port map (A => I, B => S(1),  C => S(0),  Q => D(3));
end Structural;
